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

#pragma once

#include <memory>  // for shared_ptr
#include <string>  // for string
#include <unordered_map>
#include <vector>

#include <common/error.h>        // for Error
#include <common/libev/types.h>  // for timer_id_t
#include <common/macros.h>       // for WARN_UNUSED_RESULT
#include <common/net/types.h>

#include <fastotv/protocol/types.h>

#include "server/base/iserver_handler.h"
#include "server/subscribers/rpc/user_rpc_info.h"

namespace iptv_cloud {
namespace server {
namespace subscribers {

class ProtocoledSubscriberClient;
class ServerAuthInfo;
class ISubscribeFinder;

class SubscribersHandler : public base::IServerHandler {
 public:
  typedef base::IServerHandler base_class;
  typedef ProtocoledSubscriberClient client_t;
  typedef std::unordered_map<fastotv::user_id_t, std::vector<client_t*>> inner_connections_t;
  enum {
    ping_timeout_clients = 60  // sec
  };

  explicit SubscribersHandler(ISubscribeFinder* finder, const common::net::HostAndPort& bandwidth_host);

  void PreLooped(common::libev::IoLoop* server) override;

  void Accepted(common::libev::IoClient* client) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoClient* client) override;
  void Closed(common::libev::IoClient* client) override;

  void DataReceived(common::libev::IoClient* client) override;
  void DataReadyToWrite(common::libev::IoClient* client) override;
  void PostLooped(common::libev::IoLoop* server) override;
  void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;
#if LIBEV_CHILD_ENABLE
  void Accepted(common::libev::IoChild* child) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  void ChildStatusChanged(common::libev::IoChild* client, int status) override;
#endif

  virtual ~SubscribersHandler();

 private:
  common::Error RegisterInnerConnectionByHost(const ServerAuthInfo& info,
                                              ProtocoledSubscriberClient* client) WARN_UNUSED_RESULT;
  common::Error UnRegisterInnerConnectionByHost(ProtocoledSubscriberClient* client) WARN_UNUSED_RESULT;
  std::vector<ProtocoledSubscriberClient*> FindInnerConnectionsByUser(const rpc::UserRpcInfo& user) const;

  fastotv::protocol::sequance_id_t NextRequestID();

  common::ErrnoError HandleInnerDataReceived(ProtocoledSubscriberClient* client, const std::string& input_command);
  common::ErrnoError HandleRequestCommand(ProtocoledSubscriberClient* client, fastotv::protocol::request_t* req);
  common::ErrnoError HandleResponceCommand(ProtocoledSubscriberClient* client, fastotv::protocol::response_t* resp);

  common::ErrnoError HandleRequestClientActivate(ProtocoledSubscriberClient* client, fastotv::protocol::request_t* req);
  common::ErrnoError HandleRequestClientPing(ProtocoledSubscriberClient* client, fastotv::protocol::request_t* req);
  common::ErrnoError HandleRequestClientGetServerInfo(ProtocoledSubscriberClient* client,
                                                      fastotv::protocol::request_t* req);
  common::ErrnoError HandleRequestClientGetChannels(ProtocoledSubscriberClient* client,
                                                    fastotv::protocol::request_t* req);
  common::ErrnoError HandleRequestClientGetRuntimeChannelInfo(ProtocoledSubscriberClient* client,
                                                              fastotv::protocol::request_t* req);

  common::ErrnoError HandleResponceServerPing(ProtocoledSubscriberClient* client, fastotv::protocol::response_t* resp);
  common::ErrnoError HandleResponceServerGetClientInfo(ProtocoledSubscriberClient* client,
                                                       fastotv::protocol::response_t* resp);

  size_t GetOnlineUserByStreamID(common::libev::IoLoop* server, fastotv::stream_id sid) const;

  ISubscribeFinder* finder_;
  std::atomic<fastotv::protocol::seq_id_t> id_;
  common::libev::timer_id_t ping_client_id_timer_;
  const common::net::HostAndPort bandwidth_host_;
  inner_connections_t connections_;
};

}  // namespace subscribers
}  // namespace server
}  // namespace iptv_cloud
