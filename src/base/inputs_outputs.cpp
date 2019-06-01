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

#include "base/inputs_outputs.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/convert2string.h>

#include "base/config_fields.h"

#include "utils/arg_converter.h"

#define FIELD_OUTPUT_URLS "urls"
#define FIELD_INPUT_URLS "urls"

namespace common {

bool ConvertFromString(const std::string& output_urls, iptv_cloud::output_t* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(output_urls.c_str());
  if (!obj) {
    return false;
  }

  json_object* jurls = nullptr;
  json_bool jurls_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_URLS, &jurls);
  if (!jurls_exists) {
    json_object_put(obj);
    return false;
  }

  iptv_cloud::output_t output;
  int len = json_object_array_length(jurls);
  for (int i = 0; i < len; ++i) {
    json_object* jurl = json_object_array_get_idx(jurls, i);
    iptv_cloud::OutputUri lurl;
    common::Error err = lurl.DeSerialize(jurl);
    if (!err) {
      output.push_back(lurl);
    }
  }
  json_object_put(obj);
  *out = output;
  return true;
}

bool ConvertFromString(const std::string& input_urls, iptv_cloud::input_t* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(input_urls.c_str());
  if (!obj) {
    return false;
  }

  json_object* jurls = nullptr;
  json_bool jurls_exists = json_object_object_get_ex(obj, FIELD_INPUT_URLS, &jurls);
  if (!jurls_exists) {
    json_object_put(obj);
    return false;
  }

  iptv_cloud::input_t input;
  int len = json_object_array_length(jurls);
  for (int i = 0; i < len; ++i) {
    json_object* jurl = json_object_array_get_idx(jurls, i);
    iptv_cloud::InputUri url;
    common::Error err = url.DeSerialize(jurl);
    if (!err) {
      input.push_back(url);
    }
  }
  json_object_put(obj);
  *out = input;
  return true;
}

}  // namespace common

namespace iptv_cloud {
bool read_input(const utils::ArgsMap& config, input_t* input) {
  if (!input) {
    return false;
  }

  input_t linput;
  if (!utils::ArgsGetValue(config, INPUT_FIELD, &linput)) {
    return false;
  }

  *input = linput;
  return true;
}

bool read_output(const utils::ArgsMap& config, output_t* output) {
  if (!output) {
    return false;
  }

  output_t loutput;
  if (!utils::ArgsGetValue(config, OUTPUT_FIELD, &loutput)) {
    return false;
  }

  *output = loutput;
  return true;
}

}  // namespace iptv_cloud
