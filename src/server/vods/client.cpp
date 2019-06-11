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

#include "server/vods/client.h"

namespace iptv_cloud {
namespace server {

VodsClient::VodsClient(common::libev::IoLoop* server, const common::net::socket_info& info)
    : base_class(server, info), is_verified_(false) {}

bool VodsClient::IsVerified() const {
  return is_verified_;
}

void VodsClient::SetVerified(bool verif) {
  is_verified_ = verif;
}

const char* VodsClient::ClassName() const {
  return "VodsClient";
}

}  // namespace server
}  // namespace iptv_cloud
