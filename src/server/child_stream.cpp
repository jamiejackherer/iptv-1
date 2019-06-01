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

#include "server/child_stream.h"

#include "base/stream_commands.h"
#include "base/stream_struct.h"

namespace iptv_cloud {
namespace server {

Child::Child(common::libev::IoLoop* server, Type type) : base_class(server), type_(type), client_(nullptr) {}

Child::Type Child::GetType() const {
  return type_;
}

Child::client_t* Child::GetClient() const {
  return client_;
}

void Child::SetClient(client_t* pipe) {
  client_ = pipe;
}

common::ErrnoError Child::SendStop(protocol::sequance_id_t id) {
  if (!client_) {
    return common::make_errno_error_inval();
  }

  protocol::request_t req = StopStreamRequest(id);
  return client_->WriteRequest(req);
}

common::ErrnoError Child::SendRestart(protocol::sequance_id_t id) {
  if (!client_) {
    return common::make_errno_error_inval();
  }

  protocol::request_t req = RestartStreamRequest(id);
  return client_->WriteRequest(req);
}

ChildVod::ChildVod(common::libev::IoLoop* server, stream_id_t vid) : base_class(server, VOD), vid_(vid) {}

stream_id_t ChildVod::GetStreamID() const {
  return vid_;
}

ChildStream::ChildStream(common::libev::IoLoop* server, StreamStruct* mem) : base_class(server, STREAM), mem_(mem) {}

stream_id_t ChildStream::GetStreamID() const {
  return mem_->id;
}

StreamStruct* ChildStream::GetMem() const {
  return mem_;
}

}  // namespace server
}  // namespace iptv_cloud
