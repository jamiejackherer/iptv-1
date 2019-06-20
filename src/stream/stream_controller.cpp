/*  Copyright (C) 2014-2019 FastoGT. All right reserved.
    This file is part of iptv_cloud.
    iptv_cloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    iptv_cloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with iptv_cloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stream/stream_controller.h"

#include <math.h>

#include <gst/gstcompat.h>

#include <common/file_system/string_path_utils.h>
#include <common/system_info/system_info.h>
#include <common/time.h>

#include "base/config_fields.h"  // for ID_FIELD
#include "base/constants.h"
#include "base/gst_constants.h"
#include "base/stream_commands.h"

#include "protocol/protocol.h"

#include "stream/configs_factory.h"
#include "stream/ibase_stream.h"
#include "stream/probes.h"
#include "stream/streams/configs/relay_config.h"
#include "stream/streams_factory.h"  // for isTimeshiftP...

#include "stream_commands_info/changed_sources_info.h"
#include "stream_commands_info/restart_info.h"
#include "stream_commands_info/statistic_info.h"
#include "stream_commands_info/stop_info.h"

#include "utils/arg_converter.h"

namespace iptv_cloud {
namespace stream {

namespace {

TimeShiftInfo make_timeshift_info(const utils::ArgsMap& args) {
  TimeShiftInfo tinfo;

  std::string timeshift_dir;
  if (!utils::ArgsGetValue(args, TIMESHIFT_DIR_FIELD, &timeshift_dir)) {
    CRITICAL_LOG() << "Define " TIMESHIFT_DIR_FIELD " variable and make it valid.";
  }
  tinfo.timshift_dir = common::file_system::ascii_directory_string_path(timeshift_dir);

  chunk_life_time_t timeshift_chunk_life_time = 0;
  if (utils::ArgsGetValue(args, TIMESHIFT_CHUNK_LIFE_TIME_FIELD, &timeshift_chunk_life_time)) {
    tinfo.timeshift_chunk_life_time = timeshift_chunk_life_time;
  }

  time_shift_delay_t timeshift_delay = 0;
  if (utils::ArgsGetValue(args, TIMESHIFT_DELAY_FIELD, &timeshift_delay)) {
    tinfo.timeshift_delay = timeshift_delay;
  }
  return tinfo;
}

bool PrepareStatus(StreamStruct* stats, double cpu_load, std::string* status_out) {
  if (!stats || !status_out) {
    return false;
  }

  if (isnan(cpu_load) || isinf(cpu_load)) {  // stable double
    cpu_load = 0.0;
  }

  long rss = common::system_info::GetProcessRss(getpid());
  const fastotv::timestamp_t current_time = common::time::current_utc_mstime();
  StatisticInfo sinf(*stats, cpu_load, rss * 1024, current_time);

  std::string out;
  common::Error err = sinf.SerializeToString(&out);
  if (err) {
    return false;
  }

  *status_out = out;
  return true;
}

class StreamServer : public common::libev::IoLoop {
 public:
  typedef common::libev::IoLoop base_class;
  explicit StreamServer(common::libev::IoClient* command_client, common::libev::IoLoopObserver* observer = nullptr)
      : base_class(new common::libev::LibEvLoop, observer),
        command_client_(static_cast<protocol::protocol_client_t*>(command_client)) {
    CHECK(command_client);
  }

  void WriteRequest(const protocol::request_t& request) WARN_UNUSED_RESULT {
    auto cb = [this, request] { command_client_->WriteRequest(request); };
    ExecInLoopThread(cb);
  }

  const char* ClassName() const override { return "StreamServer"; }

  common::libev::IoChild* CreateChild() override {
    NOTREACHED();
    return nullptr;
  }

  common::libev::IoClient* CreateClient(const common::net::socket_info& info) override {
    UNUSED(info);
    NOTREACHED();
    return nullptr;
  }

  void Started(common::libev::LibEvLoop* loop) override {
    RegisterClient(command_client_);
    base_class::Started(loop);
  }

  void Stopped(common::libev::LibEvLoop* loop) override {
    UnRegisterClient(command_client_);
    base_class::Stopped(loop);
  }

 private:
  protocol::protocol_client_t* const command_client_;
};

}  // namespace

StreamController::StreamController(const std::string& feedback_dir,
                                   common::libev::IoClient* command_client,
                                   StreamStruct* mem)
    : IBaseStream::IStreamClient(),
      feedback_dir_(feedback_dir),
      config_(nullptr),
      timeshift_info_(),
      restart_attempts_(0),
      stop_mutex_(),
      stop_cond_(),
      stop_(false),
      ev_thread_(),
      loop_(new StreamServer(command_client, this)),
      ttl_master_timer_(0),
      libev_started_(2),
      mem_(mem),
      origin_(nullptr),
      id_(0) {
  CHECK(mem);
  loop_->SetName("main");
}

common::Error StreamController::Init(const utils::ArgsMap& config_args) {
  Config* lconfig = nullptr;
  common::Error err = make_config(config_args, &lconfig);
  if (err) {
    return err;
  }

  config_ = lconfig;
  StreamType stream_type = config_->GetType();
  if (stream_type == TIMESHIFT_RECORDER || stream_type == TIMESHIFT_PLAYER || stream_type == CATCHUP) {
    timeshift_info_ = make_timeshift_info(config_args);
  }

  EncoderType enc = CPU;
  std::string video_codec;
  if (utils::ArgsGetValue(config_args, VIDEO_CODEC_FIELD, &video_codec)) {
    EncoderType lenc;
    if (GetEncoderType(video_codec, &lenc)) {
      enc = lenc;
    }
  }

  streams_init(0, nullptr, enc);
  return common::Error();
}

StreamController::~StreamController() {
  loop_->Stop();
  ev_thread_.join();

  destroy(&loop_);
  streams_deinit();
  destroy(&config_);
}

int StreamController::Exec() {
  ev_thread_ = std::thread([this] {
    int res = loop_->Exec();
    UNUSED(res);
  });
  libev_started_.Wait();

  while (!stop_) {
    chunk_index_t start_chunk_index = invalid_chunk_index;
    if (config_->GetType() == TIMESHIFT_PLAYER) {  // if timeshift player or cathcup player
      const streams::TimeshiftConfig* tconfig = static_cast<const streams::TimeshiftConfig*>(config_);
      time_t timeshift_chunk_duration = tconfig->GetTimeShiftChunkDuration();
      while (!timeshift_info_.FindChunkToPlay(timeshift_chunk_duration, &start_chunk_index)) {
        mem_->status = WAITING;
        DumpStreamStatus(mem_);

        {
          std::unique_lock<std::mutex> lock(stop_mutex_);
          std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(timeshift_chunk_duration));
          if (interrupt_status == std::cv_status::no_timeout) {  // if notify
            mem_->restarts++;
            break;
          }
        }
      }

      INFO_LOG() << "Founded chunk index " << start_chunk_index;
      if (start_chunk_index == invalid_chunk_index) {
        continue;
      }
    }

    int stabled_status = EXIT_SUCCESS;
    int signal_number = 0;
    fastotv::timestamp_t start_utc_now = common::time::current_utc_mstime();
    origin_ = StreamsFactory::GetInstance().CreateStream(config_, this, mem_, timeshift_info_, start_chunk_index);
    if (!origin_) {
      CRITICAL_LOG() << "Can't create stream";
      break;
    }

    bool is_vod = origin_->IsVod();
    ExitStatus res = origin_->Exec();
    destroy(&origin_);
    if (res == EXIT_INNER) {
      stabled_status = EXIT_FAILURE;
    }

    fastotv::timestamp_t end_utc_now = common::time::current_utc_mstime();
    fastotv::timestamp_t diff_utc_time = end_utc_now - start_utc_now;
    INFO_LOG() << "Stream exit with status: " << (stabled_status ? "FAILURE" : "SUCCESS")
               << ", signal: " << signal_number << ", working time: " << diff_utc_time << " msec.";
    if (is_vod) {
      break;
    }

    if (stabled_status == EXIT_SUCCESS) {
      restart_attempts_ = 0;
      continue;
    }

    if (mem_->WithoutRestartTime() > restart_after_frozen_sec * 10) {  // if longer work
      restart_attempts_ = 0;
      continue;
    }

    size_t wait_time = 0;
    if (++restart_attempts_ == config_->GetMaxRestartAttempts()) {
      restart_attempts_ = 0;
      mem_->status = FROZEN;
      DumpStreamStatus(mem_);
      wait_time = restart_after_frozen_sec;
    } else {
      wait_time = restart_attempts_ * (restart_after_frozen_sec / config_->GetMaxRestartAttempts());
    }

    INFO_LOG() << "Automatically restarted after " << wait_time << " seconds, stream restarts: " << mem_->restarts
               << ", attempts: " << restart_attempts_;

    std::unique_lock<std::mutex> lock(stop_mutex_);
    std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(wait_time));
    if (interrupt_status == std::cv_status::no_timeout) {  // if notify
      restart_attempts_ = 0;
    }
  }

  return EXIT_SUCCESS;
}

void StreamController::Stop() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_ = true;
    stop_cond_.notify_all();
  }
  StopStream();
}

void StreamController::Restart() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_cond_.notify_all();
  }
  StopStream();
}

void StreamController::PreLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);
  const auto ttl_sec = config_->GetTimeToLifeStream();
  if (ttl_sec && *ttl_sec) {
    ttl_master_timer_ = loop_->CreateTimer(*ttl_sec, false);
    NOTICE_LOG() << "Set stream ttl: " << *ttl_sec;
  }

  libev_started_.Wait();
  INFO_LOG() << "Child listening started!";
}

void StreamController::PostLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);
  if (ttl_master_timer_) {
    loop_->RemoveTimer(ttl_master_timer_);
  }
  INFO_LOG() << "Child listening finished!";
}

void StreamController::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
}

void StreamController::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void StreamController::Closed(common::libev::IoClient* client) {
  UNUSED(client);
  Stop();
}

common::ErrnoError StreamController::StreamDataRecived(common::libev::IoClient* client) {
  std::string input_command;
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  common::ErrnoError err = pclient->ReadCommand(&input_command);
  if (err) {  // i don't want handle spam, command must be formated according
              // protocol
    return err;
  }

  protocol::request_t* req = nullptr;
  protocol::response_t* resp = nullptr;
  common::Error err_parse = common::protocols::json_rpc::ParseJsonRPC(input_command, &req, &resp);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  if (req) {
    INFO_LOG() << "Received request: " << input_command;
    err = HandleRequestCommand(pclient, req);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete req;
  } else if (resp) {
    INFO_LOG() << "Received responce: " << input_command;
    err = HandleResponceCommand(pclient, resp);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete resp;
  } else {
    NOTREACHED();
    return common::make_errno_error("Invalid command type.", EINVAL);
  }
  return common::ErrnoError();
}

void StreamController::DataReceived(common::libev::IoClient* client) {
  auto err = StreamDataRecived(client);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    // client->Close();
    // delete client;
    Stop();
  }
}

void StreamController::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

void StreamController::TimerEmited(common::libev::IoLoop* loop, common::libev::timer_id_t id) {
  UNUSED(loop);
  if (id == ttl_master_timer_) {
    const auto ttl_sec = config_->GetTimeToLifeStream();
    if (ttl_sec) {
      NOTICE_LOG() << "Timeout notified ttl was: " << *ttl_sec;
    }
    Stop();
  }
}

void StreamController::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void StreamController::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void StreamController::ChildStatusChanged(common::libev::IoChild* child, int status) {
  UNUSED(child);
  UNUSED(status);
}

common::ErrnoError StreamController::HandleRequestCommand(common::libev::IoClient* client, protocol::request_t* req) {
  if (req->method == STOP_STREAM) {
    return HandleRequestStopStream(client, req);
  } else if (req->method == RESTART_STREAM) {
    return HandleRequestRestartStream(client, req);
  }

  WARNING_LOG() << "Received unknown command: " << req->method;
  return common::ErrnoError();
}

common::ErrnoError StreamController::HandleResponceCommand(common::libev::IoClient* client,
                                                           protocol::response_t* resp) {
  CHECK(loop_->IsLoopThread());

  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  protocol::request_t req;
  if (pclient->PopRequestByID(resp->id, &req)) {
    if (req.method == STATISTIC_STREAM) {
    } else if (req.method == CHANGED_SOURCES_STREAM) {
    } else {
      WARNING_LOG() << "HandleResponceStreamsCommand not handled command: " << req.method;
    }
  }
  return common::ErrnoError();
}

protocol::sequance_id_t StreamController::NextRequestID() {
  const protocol::seq_id_t next_id = id_++;
  return common::protocols::json_rpc::MakeRequestID(next_id);
}

common::ErrnoError StreamController::HandleRequestStopStream(common::libev::IoClient* client,
                                                             protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  protocol::response_t resp = StopStreamResponceSuccess(req->id);
  pclient->WriteResponce(resp);
  Stop();
  return common::ErrnoError();
}

common::ErrnoError StreamController::HandleRequestRestartStream(common::libev::IoClient* client,
                                                                protocol::request_t* req) {
  CHECK(loop_->IsLoopThread());
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  protocol::response_t resp = RestartStreamResponceSuccess(req->id);
  pclient->WriteResponce(resp);
  Restart();
  return common::ErrnoError();
}

void StreamController::StopStream() {
  if (origin_) {
    origin_->Quit(EXIT_SELF);
  }
}

void StreamController::RestartStream() {
  if (origin_) {
    origin_->Restart();
  }
}

void StreamController::OnStatusChanged(IBaseStream* stream, StreamStatus status) {
  UNUSED(status);
  DumpStreamStatus(stream->GetStats());
}

GstPadProbeInfo* StreamController::OnCheckReveivedData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

GstPadProbeInfo* StreamController::OnCheckReveivedOutputData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

void StreamController::OnProbeEvent(IBaseStream* stream, Probe* probe, GstEvent* event) {
  UNUSED(stream);
  UNUSED(probe);
  UNUSED(event);
}

void StreamController::OnPipelineEOS(IBaseStream* stream) {
  if (stream->IsVod()) {
    stream->Quit(EXIT_SELF);
  } else {
    stream->Quit(EXIT_INNER);
  }
}

void StreamController::OnTimeoutUpdated(IBaseStream* stream) {
  DumpStreamStatus(stream->GetStats());
}

void StreamController::OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void StreamController::OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void StreamController::OnInputChanged(const InputUri& uri) {
  ChangedSouresInfo ch(mem_->id, uri);
  std::string changed_json;
  common::Error err = ch.SerializeToString(&changed_json);
  if (err) {
    return;
  }

  protocol::request_t req = ChangedSourcesStreamBroadcast(changed_json);
  static_cast<StreamServer*>(loop_)->WriteRequest(req);
}

void StreamController::OnPipelineCreated(IBaseStream* stream) {
  common::file_system::ascii_directory_string_path feedback_dir(feedback_dir_);
  auto dump_file = feedback_dir.MakeFileStringPath(DUMP_FILE_NAME);
  if (dump_file) {
    stream->DumpIntoFile(*dump_file);
  }
}

void StreamController::DumpStreamStatus(StreamStruct* stat) {
  std::string status_json;
  if (PrepareStatus(stat, common::system_info::GetCpuLoad(getpid()), &status_json)) {
    protocol::request_t req = StatisticStreamBroadcast(status_json);
    static_cast<StreamServer*>(loop_)->WriteRequest(req);
  }
}

}  // namespace stream
}  // namespace iptv_cloud
