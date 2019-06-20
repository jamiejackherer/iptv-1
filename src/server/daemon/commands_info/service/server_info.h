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

#include <common/net/types.h>
#include <common/serializer/json_serializer.h>

#include <fastotv/client_server_types.h>

#include "utils/utils.h"

namespace iptv_cloud {
namespace server {
namespace service {

class OnlineUsers : public common::serializer::JsonSerializer<OnlineUsers> {
 public:
  typedef JsonSerializer<OnlineUsers> base_class;
  OnlineUsers();
  explicit OnlineUsers(size_t daemon, size_t http, size_t vods, size_t subscriber);

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  size_t daemon_;
  size_t http_;
  size_t vods_;
  size_t subscriber_;
};

class ServerInfo : public common::serializer::JsonSerializer<ServerInfo> {
 public:
  typedef JsonSerializer<ServerInfo> base_class;
  ServerInfo();
  explicit ServerInfo(int cpu_load,
                      int gpu_load,
                      const std::string& uptime,
                      const utils::MemoryShot& mem_shot,
                      const utils::HddShot& hdd_shot,
                      fastotv::bandwidth_t net_bytes_recv,
                      fastotv::bandwidth_t net_bytes_send,
                      const utils::SysinfoShot& sys,
                      fastotv::timestamp_t timestamp,
                      const OnlineUsers& online_users);

  int GetCpuLoad() const;
  int GetGpuLoad() const;
  std::string GetUptime() const;
  utils::MemoryShot GetMemShot() const;
  utils::HddShot GetHddShot() const;
  fastotv::bandwidth_t GetNetBytesRecv() const;
  fastotv::bandwidth_t GetNetBytesSend() const;
  fastotv::timestamp_t GetTimestamp() const;
  OnlineUsers GetOnlineUsers() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  int cpu_load_;
  int gpu_load_;
  std::string uptime_;
  utils::MemoryShot mem_shot_;
  utils::HddShot hdd_shot_;
  fastotv::bandwidth_t net_bytes_recv_;
  fastotv::bandwidth_t net_bytes_send_;
  fastotv::timestamp_t current_ts_;
  utils::SysinfoShot sys_shot_;
  OnlineUsers online_users_;
};

class FullServiceInfo : public ServerInfo {
 public:
  typedef ServerInfo base_class;
  FullServiceInfo();
  explicit FullServiceInfo(const common::net::HostAndPort& http_host,
                           const common::net::HostAndPort& vods_host,
                           const common::net::HostAndPort& subscribers_host,
                           const common::net::HostAndPort& bandwidth_host,
                           const base_class& base);

  common::net::HostAndPort GetHttpHost() const;
  common::net::HostAndPort GetVodsHost() const;
  common::net::HostAndPort GetSubscribersHost() const;
  common::net::HostAndPort GetBandwidthHost() const;
  std::string GetProjectVersion() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  common::net::HostAndPort http_host_;
  common::net::HostAndPort vods_host_;
  common::net::HostAndPort subscribers_host_;
  common::net::HostAndPort bandwidth_host_;
  std::string proj_ver_;
};

}  // namespace service
}  // namespace server
}  // namespace iptv_cloud
