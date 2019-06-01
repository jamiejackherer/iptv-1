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
    along with iptv_cloud. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>

#include <string>  // for string

#include <common/optional.h>

#define TS_EXTENSION "ts"
#define M3U8_EXTENSION "m3u8"
#define M3U8_CHUNK_MARKER "#EXTINF"
#define CHUNK_EXT "." TS_EXTENSION

#define DUMP_FILE_NAME "dump.html"

namespace iptv_cloud {

typedef uint64_t channel_id_t;
typedef std::string stream_id_t;
typedef common::Optional<double> volume_t;
typedef double alpha_t;
typedef common::Optional<int> bit_rate_t;

enum StreamType : uint8_t {
  RELAY = 0,
  ENCODE = 1,
  TIMESHIFT_PLAYER = 2,
  TIMESHIFT_RECORDER = 3,
  CATCHUP = 4,
  TEST_LIFE = 5,
  VOD_RELAY = 6,
  VOD_ENCODE = 7,
  SCREEN  // for inner use
};

}  // namespace iptv_cloud
