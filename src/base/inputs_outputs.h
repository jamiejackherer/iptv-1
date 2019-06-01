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

#include <common/media/types.h>

#include "base/input_uri.h"   // for InputUri
#include "base/output_uri.h"  // for OutputUri

#include "utils/arg_reader.h"  // for ArgsMap

namespace iptv_cloud {

typedef std::vector<InputUri> input_t;
typedef std::vector<OutputUri> output_t;

bool read_input(const utils::ArgsMap& config, input_t* input);
bool read_output(const utils::ArgsMap& config, output_t* output);

}  // namespace iptv_cloud

namespace common {
bool ConvertFromString(const std::string& output_urls, iptv_cloud::output_t* out);
bool ConvertFromString(const std::string& input_urls, iptv_cloud::input_t* out);
}  // namespace common
