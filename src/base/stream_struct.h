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
#include <vector>

#include <common/macros.h>

#include "base/types.h"

namespace iptv_cloud {

enum StreamStatus { NEW = 0, INIT = 1, STARTED = 2, READY = 3, PLAYING = 4, FROZEN = 5, WAITING = 6 };

class ChannelStats;

typedef std::vector<ChannelStats*> input_channels_info_t;
typedef std::vector<ChannelStats*> output_channels_info_t;

struct StreamInfo {
  stream_id_t id;
  StreamType type;
  std::vector<channel_id_t> input;
  std::vector<channel_id_t> output;
};

struct StreamStruct {
  StreamStruct();
  explicit StreamStruct(const StreamInfo& sha);
  StreamStruct(const StreamInfo& sha, time_t start_time, time_t lst, size_t rest);
  StreamStruct(stream_id_t sid,
               StreamType type,
               StreamStatus status,
               input_channels_info_t input,
               output_channels_info_t output,
               time_t start_time,
               time_t lst,
               size_t rest);

  bool IsValid() const;

  ~StreamStruct();

  time_t WithoutRestartTime() const;

  void ResetDataWait();

  const stream_id_t id;
  const StreamType type;

  const time_t start_time;  // sec

  time_t loop_start_time;
  size_t restarts;
  StreamStatus status;

  const input_channels_info_t input;    // ptrs
  const output_channels_info_t output;  // ptrs

 private:
  DISALLOW_COPY_AND_ASSIGN(StreamStruct);
};

}  // namespace iptv_cloud

namespace common {
std::string ConvertToString(iptv_cloud::StreamStatus st);
}
