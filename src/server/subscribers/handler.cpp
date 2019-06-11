/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

    This file is part of FastoTV.

    FastoTV is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoTV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoTV. If not, see <http://www.gnu.org/licenses/>.
*/

#include "server/subscribers/handler.h"

#include <common/libev/io_loop.h>  // for IoLoop

#include <fastotv/commands/commands.h>
#include <fastotv/commands_info/client_info.h>
#include <fastotv/commands_info/ping_info.h>
#include <fastotv/commands_info/runtime_channel_info.h>
#include <fastotv/commands_info/server_info.h>

#include "server/subscribers/client.h"
#include "server/subscribers/commands.h"
#include "server/subscribers/commands_info/user_info.h"
#include "server/subscribers/isubscribe_finder.h"

namespace iptv_cloud {
namespace server {

SubscribersHandler::SubscribersHandler(ISubscribeFinder* finder, const common::net::HostAndPort& bandwidth_host)
    : finder_(finder),
      id_(0),
      ping_client_id_timer_(INVALID_TIMER_ID),
      bandwidth_host_(bandwidth_host),
      connections_() {}

SubscribersHandler::~SubscribersHandler() {}

void SubscribersHandler::PreLooped(common::libev::IoLoop* server) {
  ping_client_id_timer_ = server->CreateTimer(ping_timeout_clients, true);
}

void SubscribersHandler::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void SubscribersHandler::PostLooped(common::libev::IoLoop* server) {
  if (ping_client_id_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(ping_client_id_timer_);
    ping_client_id_timer_ = INVALID_TIMER_ID;
  }
}

void SubscribersHandler::TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) {
  if (ping_client_id_timer_ == id) {
    std::vector<common::libev::IoClient*> online_clients = server->GetClients();
    for (size_t i = 0; i < online_clients.size(); ++i) {
      common::libev::IoClient* client = online_clients[i];
      ProtocoledSubscriberClient* iclient = static_cast<ProtocoledSubscriberClient*>(client);
      if (iclient) {
        std::string ping_server_json;
        fastotv::commands_info::ServerPingInfo server_ping_info;
        common::Error err_ser = server_ping_info.SerializeToString(&ping_server_json);
        if (err_ser) {
          continue;
        }

        const fastotv::protocol::request_t ping_request = PingRequest(NextRequestID(), ping_server_json);
        common::ErrnoError err = iclient->WriteRequest(ping_request);
        if (err) {
          DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
          err = client->Close();
          DCHECK(!err) << "Close client error: " << err->GetDescription();
          delete client;
        } else {
          INFO_LOG() << "Sent ping to client[" << client->GetFormatedName() << "], from server["
                     << server->GetFormatedName() << "], " << online_clients.size() << " client(s) connected.";
        }
      }
    }
  }
}

#if LIBEV_CHILD_ENABLE
void SubscribersHandler::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void SubscribersHandler::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void SubscribersHandler::ChildStatusChanged(common::libev::IoChild* client, int status) {
  UNUSED(client);
  UNUSED(status);
}
#endif

void SubscribersHandler::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
}

void SubscribersHandler::Closed(common::libev::IoClient* client) {
  ProtocoledSubscriberClient* iclient = static_cast<ProtocoledSubscriberClient*>(client);
  const ServerAuthInfo server_user_auth = iclient->GetServerHostInfo();
  common::Error unreg_err = UnRegisterInnerConnectionByHost(iclient);
  if (unreg_err) {
    return;
  }

  INFO_LOG() << "Byu registered user: " << server_user_auth.GetLogin();
}

void SubscribersHandler::DataReceived(common::libev::IoClient* client) {
  std::string buff;
  ProtocoledSubscriberClient* iclient = static_cast<ProtocoledSubscriberClient*>(client);
  common::ErrnoError err = iclient->ReadCommand(&buff);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    err = client->Close();
    DCHECK(!err) << "Close client error: " << err->GetDescription();
    delete client;
    return;
  }

  HandleInnerDataReceived(iclient, buff);
}

void SubscribersHandler::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

common::Error SubscribersHandler::RegisterInnerConnectionByHost(const ServerAuthInfo& info,
                                                                ProtocoledSubscriberClient* client) {
  CHECK(info.IsValid());
  if (!client) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  client->SetServerHostInfo(info);
  connections_[info.GetUserID()].push_back(client);
  return common::Error();
}

common::Error SubscribersHandler::UnRegisterInnerConnectionByHost(ProtocoledSubscriberClient* client) {
  if (!client) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  const auto sinf = client->GetServerHostInfo();
  if (!sinf.IsValid()) {
    return common::make_error_inval();
  }

  auto hs = connections_.find(sinf.GetUserID());
  if (hs == connections_.end()) {
    return common::Error();
  }

  for (auto it = hs->second.begin(); it != hs->second.end(); ++it) {
    if (*it == client) {
      hs->second.erase(it);
      break;
    }
  }

  if (hs->second.empty()) {
    connections_.erase(hs);
  }
  return common::Error();
}

ProtocoledSubscriberClient* SubscribersHandler::FindInnerConnectionByUser(const rpc::UserRpcInfo& user) const {
  const auto hs = connections_.find(user.GetUserID());
  if (hs == connections_.end()) {
    return nullptr;
  }

  auto devices = hs->second;
  for (client_t* connected_device : devices) {
    auto uinf = connected_device->GetServerHostInfo();
    if (uinf.MakeUserRpc() == user) {
      return connected_device;
    }
  }
  return nullptr;
}

size_t SubscribersHandler::GetOnlineUserByStreamID(common::libev::IoLoop* server, fastotv::stream_id sid) const {
  size_t total = 0;
  std::vector<common::libev::IoClient*> online_clients = server->GetClients();
  for (size_t i = 0; i < online_clients.size(); ++i) {
    common::libev::IoClient* client = online_clients[i];
    ProtocoledSubscriberClient* iclient = static_cast<ProtocoledSubscriberClient*>(client);
    if (iclient && iclient->GetCurrentStreamID() == sid) {
      total++;
    }
  }

  return total;
}

common::ErrnoError SubscribersHandler::HandleRequestClientActivate(ProtocoledSubscriberClient* client,
                                                                   fastotv::protocol::request_t* req) {
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jauth = json_tokener_parse(params_ptr);
    if (!jauth) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::AuthInfo uauth;
    common::Error err_des = uauth.DeSerialize(jauth);
    json_object_put(jauth);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      fastotv::protocol::response_t resp = ActivateResponseFail(req->id, err_str);
      client->WriteResponce(resp);
      return common::make_errno_error(err_str, EAGAIN);
    }

    if (!uauth.IsValid()) {
      return common::make_errno_error(EAGAIN);
    }

    UserInfo registered_user;
    common::Error err_find = finder_->FindUser(uauth, &registered_user);
    if (err_find) {
      return common::make_errno_error(EAGAIN);
    }

    const fastotv::device_id_t dev = uauth.GetDeviceID();
    if (!registered_user.HaveDevice(dev)) {
      const std::string error_str = "Unknown device reject";
      fastotv::protocol::response_t resp = ActivateResponseFail(req->id, error_str);
      client->WriteResponce(resp);
      return common::make_errno_error(error_str, EINVAL);
    }

    if (registered_user.IsBanned()) {
      const std::string error_str = "Banned user";
      fastotv::protocol::response_t resp = ActivateResponseFail(req->id, error_str);
      client->WriteResponce(resp);
      return common::make_errno_error(error_str, EINVAL);
    }

    const ServerAuthInfo server_user_auth(registered_user.GetUserID(), uauth);
    const rpc::UserRpcInfo user_rpc = server_user_auth.MakeUserRpc();
    ProtocoledSubscriberClient* fclient = FindInnerConnectionByUser(user_rpc);
    if (fclient) {
      const std::string error_str = "Double connection reject";
      fastotv::protocol::response_t resp = ActivateResponseFail(req->id, error_str);
      client->WriteResponce(resp);
      return common::make_errno_error(error_str, EINVAL);
    }

    const fastotv::protocol::response_t resp = ActivateResponseSuccess(req->id);
    common::ErrnoError errn = client->WriteResponce(resp);
    if (errn) {
      return errn;
    }

    common::Error err = RegisterInnerConnectionByHost(server_user_auth, client);
    CHECK(!err) << "Register inner connection error: " << err->GetDescription();
    INFO_LOG() << "Welcome registered user: " << uauth.GetLogin();
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError SubscribersHandler::HandleRequestClientPing(ProtocoledSubscriberClient* client,
                                                               fastotv::protocol::request_t* req) {
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jstop = json_tokener_parse(params_ptr);
    if (!jstop) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::ClientPingInfo ping_info;
    common::Error err_des = ping_info.DeSerialize(jstop);
    json_object_put(jstop);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    fastotv::protocol::response_t resp = PingResponseSuccess(req->id);
    return client->WriteResponce(resp);
  }

  return common::make_errno_error_inval();
}

common::ErrnoError SubscribersHandler::HandleRequestClientGetServerInfo(ProtocoledSubscriberClient* client,
                                                                        fastotv::protocol::request_t* req) {
  fastotv::commands_info::AuthInfo hinf = client->GetServerHostInfo();
  UserInfo user;
  common::Error err = finder_->FindUser(hinf, &user);
  if (err) {
    const fastotv::protocol::response_t resp = GetServerInfoResponceFail(req->id, err->GetDescription());
    ignore_result(client->WriteResponce(resp));
    ignore_result(client->Close());
    delete client;
    const std::string err_str = err->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  fastotv::commands_info::ServerInfo serv(bandwidth_host_);
  std::string server_info_str;
  common::Error err_ser = serv.SerializeToString(&server_info_str);
  if (err_ser) {
    const std::string err_str = err_ser->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  const fastotv::protocol::response_t server_info_responce = GetServerInfoResponceSuccsess(req->id, server_info_str);
  return client->WriteResponce(server_info_responce);
}

common::ErrnoError SubscribersHandler::HandleRequestClientGetChannels(ProtocoledSubscriberClient* client,
                                                                      fastotv::protocol::request_t* req) {
  fastotv::commands_info::AuthInfo hinf = client->GetServerHostInfo();
  UserInfo user;
  common::Error err = finder_->FindUser(hinf, &user);
  if (err) {
    const std::string err_str = err->GetDescription();
    const fastotv::protocol::response_t resp = GetServerInfoResponceFail(req->id, err_str);
    ignore_result(client->WriteResponce(resp));
    ignore_result(client->Close());
    delete client;
    return common::make_errno_error(err_str, EAGAIN);
  }

  std::string channels_str;
  fastotv::commands_info::ChannelsInfo chan = user.GetChannelInfo();
  common::Error err_ser = chan.SerializeToString(&channels_str);
  if (err_ser) {
    const std::string err_str = err_ser->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  const fastotv::protocol::response_t channels_responce = GetChannelsResponceSuccsess(req->id, channels_str);
  return client->WriteResponce(channels_responce);
}

common::ErrnoError SubscribersHandler::HandleRequestClientGetRuntimeChannelInfo(ProtocoledSubscriberClient* client,
                                                                                fastotv::protocol::request_t* req) {
  if (req->params) {
    const char* params_ptr = req->params->c_str();
    json_object* jrun = json_tokener_parse(params_ptr);
    if (!jrun) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::RuntimeChannelLiteInfo run;
    common::Error err_des = run.DeSerialize(jrun);
    json_object_put(jrun);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    common::libev::IoLoop* server = client->GetServer();
    const fastotv::stream_id channel = run.GetChannelID();

    size_t watchers = GetOnlineUserByStreamID(server, channel);  // calc watchers
    client->SetCurrentStreamID(channel);                         // add to watcher

    fastotv::commands_info::RuntimeChannelInfo rinf;
    rinf.SetChannelID(channel);
    rinf.SetWatchersCount(watchers);
    rinf.SetChannelType(fastotv::OFFICAL_CHANNEL);

    std::string rchannel_str;
    common::Error err_ser = rinf.SerializeToString(&rchannel_str);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    const fastotv::protocol::response_t channels_responce = GetRuntimeChannelInfoResponceSuccsess(req->id, rchannel_str);
    common::ErrnoError err = client->WriteResponce(channels_responce);
    if (err) {
      return err;
    }

    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError SubscribersHandler::HandleInnerDataReceived(ProtocoledSubscriberClient* client,
                                                               const std::string& input_command) {
  fastotv::protocol::request_t* req = nullptr;
  fastotv::protocol::response_t* resp = nullptr;
  common::Error err_parse = common::protocols::json_rpc::ParseJsonRPC(input_command, &req, &resp);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  if (req) {
    INFO_LOG() << "Received request: " << input_command;
    common::ErrnoError err = HandleRequestCommand(client, req);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete req;
  } else if (resp) {
    INFO_LOG() << "Received responce: " << input_command;
    common::ErrnoError err = HandleResponceCommand(client, resp);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
    delete resp;
  } else {
    DNOTREACHED();
    return common::make_errno_error("Invalid command type.", EINVAL);
  }

  return common::ErrnoError();
}

fastotv::protocol::sequance_id_t SubscribersHandler::NextRequestID() {
  const fastotv::protocol::seq_id_t next_id = id_++;
  return common::protocols::json_rpc::MakeRequestID(next_id);
}

common::ErrnoError SubscribersHandler::HandleRequestCommand(ProtocoledSubscriberClient* client,
                                                            fastotv::protocol::request_t* req) {
  ProtocoledSubscriberClient* iclient = static_cast<ProtocoledSubscriberClient*>(client);
  if (req->method == CLIENT_ACTIVATE) {
    return HandleRequestClientActivate(iclient, req);
  } else if (req->method == CLIENT_PING) {
    return HandleRequestClientPing(iclient, req);
  } else if (req->method == CLIENT_GET_SERVER_INFO) {
    return HandleRequestClientGetServerInfo(iclient, req);
  } else if (req->method == CLIENT_GET_CHANNELS) {
    return HandleRequestClientGetChannels(iclient, req);
  } else if (req->method == CLIENT_GET_RUNTIME_CHANNEL_INFO) {
    return HandleRequestClientGetRuntimeChannelInfo(iclient, req);
  }

  WARNING_LOG() << "Received unknown command: " << req->method;
  return common::ErrnoError();
}

common::ErrnoError SubscribersHandler::HandleResponceServerPing(ProtocoledSubscriberClient* client,
                                                                fastotv::protocol::response_t* resp) {
  UNUSED(client);
  if (resp->IsMessage()) {
    const char* params_ptr = resp->message->result.c_str();
    json_object* jclient_ping = json_tokener_parse(params_ptr);
    if (!jclient_ping) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::ServerPingInfo server_ping_info;
    common::Error err_des = server_ping_info.DeSerialize(jclient_ping);
    json_object_put(jclient_ping);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }
    return common::ErrnoError();
  }
  return common::ErrnoError();
}

common::ErrnoError SubscribersHandler::HandleResponceServerGetClientInfo(ProtocoledSubscriberClient* client,
                                                                         fastotv::protocol::response_t* resp) {
  UNUSED(client);
  if (resp->IsMessage()) {
    const char* params_ptr = resp->message->result.c_str();
    json_object* jclient_info = json_tokener_parse(params_ptr);
    if (!jclient_info) {
      return common::make_errno_error_inval();
    }

    fastotv::commands_info::ClientInfo cinf;
    common::Error err_des = cinf.DeSerialize(jclient_info);
    json_object_put(jclient_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }
    return common::ErrnoError();
  }
  return common::ErrnoError();
}

common::ErrnoError SubscribersHandler::HandleResponceCommand(ProtocoledSubscriberClient* client,
                                                             fastotv::protocol::response_t* resp) {
  fastotv::protocol::request_t req;
  ProtocoledSubscriberClient* sclient = static_cast<ProtocoledSubscriberClient*>(client);
  ProtocoledSubscriberClient::callback_t cb;
  if (sclient->PopRequestByID(resp->id, &req, &cb)) {
    if (cb) {
      cb(resp);
    }
    if (req.method == SERVER_PING) {
      return HandleResponceServerPing(sclient, resp);
    } else if (req.method == SERVER_GET_CLIENT_INFO) {
      return HandleResponceServerGetClientInfo(sclient, resp);
    } else {
      WARNING_LOG() << "HandleResponceServiceCommand not handled command: " << req.method;
    }
  }

  return common::ErrnoError();
}

}  // namespace server
}  // namespace iptv_cloud
