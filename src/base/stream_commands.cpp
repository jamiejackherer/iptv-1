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

#include "base/stream_commands.h"

namespace iptv_cloud {

protocol::request_t RestartStreamRequest(protocol::sequance_id_t id) {
  protocol::request_t req;
  req.id = id;
  req.method = RESTART_STREAM;
  return req;
}

protocol::response_t RestartStreamResponceSuccess(protocol::sequance_id_t id) {
  return protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
}

protocol::request_t StopStreamRequest(protocol::sequance_id_t id) {
  protocol::request_t req;
  req.id = id;
  req.method = STOP_STREAM;
  return req;
}

protocol::response_t StopStreamResponceSuccess(protocol::sequance_id_t id) {
  return protocol::response_t::MakeMessage(id, common::protocols::json_rpc::JsonRPCMessage::MakeSuccessMessage());
}

protocol::request_t ChangedSourcesStreamBroadcast(protocol::serializet_params_t params) {
  return protocol::request_t::MakeNotification(CHANGED_SOURCES_STREAM, params);
}

protocol::request_t StatisticStreamBroadcast(protocol::serializet_params_t params) {
  return protocol::request_t::MakeNotification(STATISTIC_STREAM, params);
}

}  // namespace iptv_cloud
