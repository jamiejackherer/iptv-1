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

#include <common/libev/io_child.h>

#include "protocol/protocol.h"
#include "protocol/types.h"

#include "base/types.h"

namespace iptv_cloud {
struct StreamStruct;
namespace server {

class Child : public common::libev::IoChild {
 public:
  enum Type : uint8_t { VOD = 0, STREAM };

  typedef common::libev::IoChild base_class;
  typedef protocol::protocol_client_t client_t;
  Child(common::libev::IoLoop* server, Type type);

  virtual stream_id_t GetStreamID() const = 0;
  Type GetType() const;

  common::ErrnoError SendStop(protocol::sequance_id_t id) WARN_UNUSED_RESULT;
  common::ErrnoError SendRestart(protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  client_t* GetClient() const;
  void SetClient(client_t* pipe);

 private:
  Type type_;
  client_t* client_;
};

class ChildVod : public Child {
 public:
  typedef Child base_class;
  ChildVod(common::libev::IoLoop* server, stream_id_t vid);

  stream_id_t GetStreamID() const override;

 private:
  const stream_id_t vid_;
};

class ChildStream : public Child {
 public:
  typedef Child base_class;
  ChildStream(common::libev::IoLoop* server, StreamStruct* mem);

  stream_id_t GetStreamID() const override;

  StreamStruct* GetMem() const;

 private:
  StreamStruct* const mem_;

  DISALLOW_COPY_AND_ASSIGN(ChildStream);
};

}  // namespace server
}  // namespace iptv_cloud
