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

#include "base/inputs_outputs.h"

namespace iptv_cloud {
namespace stream {

class Config {
 public:
  enum { report_delay_sec = 10 };
  typedef common::Optional<time_t> ttl_t;
  Config(StreamType type, size_t max_restart_attempts, const input_t& input, const output_t& output);
  StreamType GetType() const;

  bool Equals(const Config& api) const;

  input_t GetInput() const;  // all except timeshift_play
  void SetInput(const input_t& input);

  output_t GetOutput() const;  // all except timeshift_rec
  void SetOutput(const output_t& output);

  size_t GetMaxRestartAttempts() const;
  void SetMaxRestartAttempts(size_t attempts);

  ttl_t GetTimeToLifeStream() const;
  void SetTimeToLigeStream(ttl_t ttl);

 private:
  StreamType type_;
  size_t max_restart_attempts_;
  ttl_t ttl_sec_;

  input_t input_;
  output_t output_;
};

inline bool operator==(const Config& left, const Config& right) {
  return left.Equals(right);
}

inline bool operator!=(const Config& left, const Config& right) {
  return !operator==(left, right);
}

}  // namespace stream
}  // namespace iptv_cloud
