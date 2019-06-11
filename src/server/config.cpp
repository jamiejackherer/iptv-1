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

#include "server/config.h"

#include <fstream>
#include <utility>

#include "utils/arg_converter.h"

#define SERVICE_LOG_PATH_FIELD "log_path"
#define SERVICE_LOG_LEVEL_FIELD "log_level"
#define SERVICE_HOST_FIELD "host"
#define SERVICE_HTTP_HOST_FIELD "http_host"
#define SERVICE_VODS_HOST_FIELD "vods_host"
#define SERVICE_SUBSCRIBERS_HOST_FIELD "subscribers_host"
#define SERVICE_BANDWIDTH_HOST_FIELD "bandwidth_host"
#define SERVICE_TTL_FILES_FIELD "ttl_files"

#define DUMMY_LOG_FILE_PATH "/dev/null"

namespace {
common::ErrnoError ReadSlaveConfig(const std::string& path, iptv_cloud::utils::ArgsMap* args) {
  if (!args) {
    return common::make_errno_error_inval();
  }

  if (path.empty()) {
    return common::make_errno_error("Invalid config path", EINVAL);
  }

  std::ifstream config(path);
  if (!config.is_open()) {
    return common::make_errno_error("Failed to open config path: " + path, EINVAL);
  }

  iptv_cloud::utils::ArgsMap options;
  std::string line;
  while (getline(config, line)) {
    const std::pair<std::string, std::string> pair = iptv_cloud::utils::GetKeyValue(line, '=');
    if (pair.first == SERVICE_LOG_PATH_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_LOG_LEVEL_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_HOST_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_HTTP_HOST_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_VODS_HOST_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_SUBSCRIBERS_HOST_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_BANDWIDTH_HOST_FIELD) {
      options.insert(pair);
    } else if (pair.first == SERVICE_TTL_FILES_FIELD) {
      options.insert(pair);
    }
  }

  *args = options;
  return common::ErrnoError();
}

}  // namespace

namespace iptv_cloud {
namespace server {

Config::Config()
    : host(GetDefaultHost()),
      log_path(DUMMY_LOG_FILE_PATH),
      log_level(common::logging::LOG_LEVEL_INFO),
      ttl_files_(TTL_FILES) {}

common::net::HostAndPort Config::GetDefaultHost() {
  return common::net::HostAndPort::CreateLocalHost(CLIENT_PORT);
}

common::ErrnoError load_config_from_file(const std::string& config_absolute_path, Config* config) {
  if (!config) {
    return common::make_errno_error_inval();
  }

  Config lconfig;
  utils::ArgsMap slave_config_args;
  common::ErrnoError err = ReadSlaveConfig(config_absolute_path, &slave_config_args);
  if (err) {
    return err;
  }

  std::string log_path;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_LOG_PATH_FIELD, &log_path)) {
    log_path = DUMMY_LOG_FILE_PATH;
  }
  lconfig.log_path = log_path;

  std::string level_str;
  common::logging::LOG_LEVEL level = common::logging::LOG_LEVEL_INFO;
  if (utils::ArgsGetValue(slave_config_args, SERVICE_LOG_LEVEL_FIELD, &level_str)) {
    if (!common::logging::text_to_log_level(level_str.c_str(), &level)) {
      level = common::logging::LOG_LEVEL_INFO;
    }
  }
  lconfig.log_level = level;

  common::net::HostAndPort host;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_HOST_FIELD, &host)) {
    host = common::net::HostAndPort::CreateLocalHost(CLIENT_PORT);
  }
  lconfig.host = host;

  common::net::HostAndPort http_host;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_HTTP_HOST_FIELD, &http_host)) {
    http_host = common::net::HostAndPort::CreateLocalHost(HTTP_PORT);
  }
  lconfig.http_host = http_host;

  common::net::HostAndPort vods_host;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_VODS_HOST_FIELD, &vods_host)) {
    vods_host = common::net::HostAndPort::CreateLocalHost(VODS_PORT);
  }
  lconfig.vods_host = vods_host;

  common::net::HostAndPort subscribers_host;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_SUBSCRIBERS_HOST_FIELD, &subscribers_host)) {
    subscribers_host = common::net::HostAndPort::CreateLocalHost(SUBSCRIPERS_PORT);
  }
  lconfig.subscribers_host = subscribers_host;

  common::net::HostAndPort bandwidth_host;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_BANDWIDTH_HOST_FIELD, &bandwidth_host)) {
    subscribers_host = common::net::HostAndPort::CreateLocalHost(BANDWIDTH_PORT);
  }
  lconfig.bandwidth_host = bandwidth_host;

  time_t ttl_files;
  if (!utils::ArgsGetValue(slave_config_args, SERVICE_TTL_FILES_FIELD, &ttl_files)) {
    ttl_files = TTL_FILES;
  }
  lconfig.ttl_files_ = ttl_files;

  *config = lconfig;
  return common::ErrnoError();
}

}  // namespace server
}  // namespace iptv_cloud
