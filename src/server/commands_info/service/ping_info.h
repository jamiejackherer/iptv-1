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

#include <common/time.h>

#include <common/serializer/json_serializer.h>

namespace iptv_cloud {
namespace server {
namespace service {

typedef common::time64_t timestamp_t;

class ServerPingInfo : public common::serializer::JsonSerializer<ServerPingInfo> {
 public:
  ServerPingInfo();

  timestamp_t GetTimeStamp() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  timestamp_t timestamp_;  // utc time
};

class ClientPingInfo : public common::serializer::JsonSerializer<ClientPingInfo> {
 public:
  ClientPingInfo();

  timestamp_t GetTimeStamp() const;

 protected:
  common::Error DoDeSerialize(json_object* serialized) override;
  common::Error SerializeFields(json_object* out) const override;

 private:
  timestamp_t timestamp_;  // utc time
};

}  // namespace service
}  // namespace server
}  // namespace iptv_cloud
