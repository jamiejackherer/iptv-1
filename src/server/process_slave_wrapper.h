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

#pragma once

#include <map>
#include <string>

#include <common/net/types.h>

#include "base/types.h"
#include "protocol/types.h"
#include "utils/arg_reader.h"

#include "server/base/ihttp_requests_observer.h"
#include "server/base/iserver_handler.h"
#include "server/config.h"
#include "server/daemon/commands_info/stream/start_info.h"

namespace iptv_cloud {
namespace server {
namespace pipe {
class ProtocoledPipeClient;
}
namespace subscribers {
class ISubscribeFinder;
}
class Child;
class ProtocoledDaemonClient;

class ProcessSlaveWrapper : public base::IServerHandler, public server::base::IHttpRequestsObserver {
 public:
  enum { node_stats_send_seconds = 10, ping_timeout_clients_seconds = 60, cleanup_seconds = 3 };
  typedef utils::ArgsMap serialized_stream_t;

  explicit ProcessSlaveWrapper(const std::string& licensy_key, const Config& config);
  ~ProcessSlaveWrapper() override;

  static int SendStopDaemonRequest(const std::string& license);
  common::net::HostAndPort GetServerHostAndPort();

  int Exec(int argc, char** argv) WARN_UNUSED_RESULT;

 protected:
  void PreLooped(common::libev::IoLoop* server) override;
  void Accepted(common::libev::IoClient* client) override;
  void Moved(common::libev::IoLoop* server,
             common::libev::IoClient* client) override;  // owner server, now client is orphan
  void Closed(common::libev::IoClient* client) override;
  void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;

  void Accepted(common::libev::IoChild* child) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  void ChildStatusChanged(common::libev::IoChild* child, int status) override;

  void DataReceived(common::libev::IoClient* client) override;
  void DataReadyToWrite(common::libev::IoClient* client) override;
  void PostLooped(common::libev::IoLoop* server) override;

  void OnHttpRequest(common::libev::http::HttpClient* client, const file_path_t& file) override;

  virtual common::ErrnoError HandleRequestServiceCommand(ProtocoledDaemonClient* dclient,
                                                         protocol::request_t* req) WARN_UNUSED_RESULT;
  virtual common::ErrnoError HandleResponceServiceCommand(ProtocoledDaemonClient* dclient,
                                                          protocol::response_t* resp) WARN_UNUSED_RESULT;

  virtual common::ErrnoError HandleRequestStreamsCommand(pipe::ProtocoledPipeClient* pclient,
                                                         protocol::request_t* req) WARN_UNUSED_RESULT;
  virtual common::ErrnoError HandleResponceStreamsCommand(pipe::ProtocoledPipeClient* pclient,
                                                          protocol::response_t* resp) WARN_UNUSED_RESULT;

 private:
  typedef int (*stream_exec_t)(const char* process_name,
                               const void* cmd_args,
                               const void* config_args,
                               void* command_client,
                               void* mem);

  Child* FindChildByID(stream_id_t cid) const;
  void BroadcastClients(const protocol::request_t& req);

  common::ErrnoError DaemonDataReceived(ProtocoledDaemonClient* dclient) WARN_UNUSED_RESULT;
  common::ErrnoError PipeDataReceived(pipe::ProtocoledPipeClient* pclient) WARN_UNUSED_RESULT;

  protocol::sequance_id_t NextRequestID();

  common::ErrnoError CreateChildStream(const stream::StartInfo& start_info);
  common::ErrnoError CreateChildStream(const std::string& config);
  common::ErrnoError CreateChildStream(const serialized_stream_t& config_args);

  // stream
  common::ErrnoError HandleRequestChangedSourcesStream(pipe::ProtocoledPipeClient* pclient,
                                                       protocol::request_t* req) WARN_UNUSED_RESULT;

  common::ErrnoError HandleRequestStatisticStream(pipe::ProtocoledPipeClient* pclient,
                                                  protocol::request_t* req) WARN_UNUSED_RESULT;

  common::ErrnoError HandleRequestClientStartStream(ProtocoledDaemonClient* dclient,
                                                    protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientStopStream(ProtocoledDaemonClient* dclient,
                                                   protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientRestartStream(ProtocoledDaemonClient* dclient,
                                                      protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetLogStream(ProtocoledDaemonClient* dclient,
                                                     protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetPipelineStream(ProtocoledDaemonClient* dclient,
                                                          protocol::request_t* req) WARN_UNUSED_RESULT;

  // service
  common::ErrnoError HandleRequestClientPrepareService(ProtocoledDaemonClient* dclient,
                                                       protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientSyncService(ProtocoledDaemonClient* dclient,
                                                    protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientActivate(ProtocoledDaemonClient* dclient,
                                                 protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientPingService(ProtocoledDaemonClient* dclient,
                                                    protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientGetLogService(ProtocoledDaemonClient* dclient,
                                                      protocol::request_t* req) WARN_UNUSED_RESULT;
  common::ErrnoError HandleRequestClientStopService(ProtocoledDaemonClient* dclient,
                                                    protocol::request_t* req) WARN_UNUSED_RESULT;

  common::ErrnoError HandleResponcePingService(ProtocoledDaemonClient* dclient,
                                               protocol::response_t* resp) WARN_UNUSED_RESULT;

  std::string MakeServiceStats(bool full_stat) const;
  void AddStreamLine(const std::string& config);

  struct NodeStats;

  const Config config_;
  const std::string license_key_;

  int process_argc_;
  char** process_argv_;

  common::libev::IoLoop* loop_;
  common::libev::IoLoop* http_server_;
  common::libev::IoLoopObserver* http_handler_;
  common::libev::IoLoop* vods_server_;
  common::libev::IoLoopObserver* vods_handler_;
  common::libev::IoLoop* subscribers_server_;
  common::libev::IoLoopObserver* subscribers_handler_;

  std::atomic<protocol::seq_id_t> id_;
  common::libev::timer_id_t ping_client_timer_;
  common::libev::timer_id_t node_stats_timer_;
  common::libev::timer_id_t cleanup_files_timer_;
  common::libev::timer_id_t quit_cleanup_timer_;
  NodeStats* node_stats_;
  stream_exec_t stream_exec_func_;

  std::map<common::file_system::ascii_directory_string_path, serialized_stream_t> vods_links_;
  subscribers::ISubscribeFinder* finder_;
};

}  // namespace server
}  // namespace iptv_cloud
