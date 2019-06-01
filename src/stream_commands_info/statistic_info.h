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

#include <memory>

#include <common/serializer/json_serializer.h>

#include "base/stream_struct.h"

namespace iptv_cloud {

class StatisticInfo : public common::serializer::JsonSerializer<StatisticInfo> {
 public:
  typedef JsonSerializer<StatisticInfo> base_class;
  typedef double cpu_load_t;
  typedef long rss_t;
  typedef std::shared_ptr<StreamStruct> stream_struct_t;

  StatisticInfo();
  StatisticInfo(const StreamStruct& str, cpu_load_t cpu_load, rss_t rss, time_t time);

  stream_struct_t GetStreamStruct() const;
  cpu_load_t GetCpuLoad() const;
  rss_t GetRss() const;
  time_t GetTimestamp() const;

 protected:
  common::Error SerializeFields(json_object* out) const override;
  common::Error DoDeSerialize(json_object* serialized) override;

 private:
  stream_struct_t stream_struct_;
  cpu_load_t cpu_load_;
  rss_t rss_;
  time_t timestamp_;
};

}  // namespace iptv_cloud
