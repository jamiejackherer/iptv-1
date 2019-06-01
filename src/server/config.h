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

#include <string>

#include <common/error.h>
#include <common/net/types.h>

namespace iptv_cloud {
namespace server {

struct Config {
  Config();

  static common::net::HostAndPort GetDefaultHost();

  common::net::HostAndPort host;
  std::string log_path;
  common::logging::LOG_LEVEL log_level;
  common::net::HostAndPort http_host;
  common::net::HostAndPort vods_host;
  time_t ttl_files_;
};

common::ErrnoError load_config_from_file(const std::string& config_absolute_path, Config* config) WARN_UNUSED_RESULT;

}  // namespace server
}  // namespace iptv_cloud
