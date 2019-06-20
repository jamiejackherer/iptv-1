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

#include <common/libev/tcp/tcp_client.h>  // for TcpClient

#include "protocol/protocol.h"

namespace iptv_cloud {
namespace server {

class DaemonClient : public common::libev::tcp::TcpClient {
 public:
  typedef common::libev::tcp::TcpClient base_class;

  bool IsVerified() const;
  void SetVerified(bool verified);

  const char* ClassName() const override;

 protected:
  DaemonClient(common::libev::IoLoop* server, const common::net::socket_info& info);

 private:
  bool is_verified_;
};

class ProtocoledDaemonClient : public protocol::ProtocolClient<DaemonClient> {
 public:
  typedef protocol::ProtocolClient<DaemonClient> base_class;
  ProtocoledDaemonClient(common::libev::IoLoop* server, const common::net::socket_info& info);
};

}  // namespace server
}  // namespace iptv_cloud
