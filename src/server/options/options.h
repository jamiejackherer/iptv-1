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
#include <utility>

#include "utils/arg_reader.h"

namespace iptv_cloud {
namespace server {
namespace options {

enum Validity { VALID, INVALID, FATAL };

typedef Validity (*validate_callback_t)(const std::string&);

typedef std::pair<std::string, validate_callback_t> option_t;

utils::ArgsMap ValidateConfig(const std::string& config);

bool FindOption(const std::string& key, option_t* opt);

}  // namespace options
}  // namespace server
}  // namespace iptv_cloud
