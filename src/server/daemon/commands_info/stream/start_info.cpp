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

#include "server/daemon/commands_info/stream/start_info.h"

#define START_STREAM_INFO_CONFIG_KEY_FIELD "config"

namespace iptv_cloud {
namespace server {
namespace stream {

StartInfo::StartInfo() : base_class(), config_() {}

std::string StartInfo::GetConfig() const {
  return config_;
}

common::Error StartInfo::DoDeSerialize(json_object* serialized) {
  if (!serialized) {
    return common::make_error_inval();
  }

  json_object* jconfig = nullptr;
  json_bool jconfig_exists = json_object_object_get_ex(serialized, START_STREAM_INFO_CONFIG_KEY_FIELD, &jconfig);
  if (!jconfig_exists) {
    return common::make_error_inval();
  }

  StartInfo inf;
  inf.config_ = json_object_get_string(jconfig);
  *this = inf;
  return common::Error();
}

common::Error StartInfo::SerializeFields(json_object* out) const {
  json_object_object_add(out, START_STREAM_INFO_CONFIG_KEY_FIELD, json_object_new_string(config_.c_str()));
  return common::Error();
}

}  // namespace stream
}  // namespace server
}  // namespace iptv_cloud
