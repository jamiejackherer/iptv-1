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

#include "base/output_uri.h"

#include <string>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "base/constants.h"

#define FIELD_OUTPUT_ID "id"
#define FIELD_OUTPUT_URI "uri"
#define FIELD_OUTPUT_HTTP_ROOT "http_root"

namespace iptv_cloud {

OutputUri::OutputUri() : OutputUri(0, common::uri::Url()) {}

OutputUri::OutputUri(uri_id_t id, const common::uri::Url& output)
    : base_class(), id_(id), output_(output), http_root_() {}

OutputUri::uri_id_t OutputUri::GetID() const {
  return id_;
}

void OutputUri::SetID(uri_id_t id) {
  id_ = id;
}

common::uri::Url OutputUri::GetOutput() const {
  return output_;
}

void OutputUri::SetOutput(const common::uri::Url& uri) {
  output_ = uri;
}

OutputUri::http_root_t OutputUri::GetHttpRoot() const {
  return http_root_;
}

void OutputUri::SetHttpRoot(const http_root_t& root) {
  http_root_ = root;
}

bool OutputUri::Equals(const OutputUri& inf) const {
  return id_ == inf.id_ && output_ == inf.output_ && http_root_ == inf.http_root_;
}

common::Error OutputUri::DoDeSerialize(json_object* serialized) {
  OutputUri res;
  json_object* jid = nullptr;
  json_bool jid_exists = json_object_object_get_ex(serialized, FIELD_OUTPUT_ID, &jid);
  if (jid_exists) {
    res.SetID(json_object_get_int64(jid));
  }

  json_object* juri = nullptr;
  json_bool juri_exists = json_object_object_get_ex(serialized, FIELD_OUTPUT_URI, &juri);
  if (juri_exists) {
    res.SetOutput(common::uri::Url(json_object_get_string(juri)));
  }

  json_object* jhttp_root = nullptr;
  json_bool jhttp_root_exists = json_object_object_get_ex(serialized, FIELD_OUTPUT_HTTP_ROOT, &jhttp_root);
  if (jhttp_root_exists) {
    const char* http_root_str = json_object_get_string(jhttp_root);
    const common::file_system::ascii_directory_string_path http_root(http_root_str);
    res.SetHttpRoot(http_root);
  }

  *this = res;
  return common::Error();
}

common::Error OutputUri::SerializeFields(json_object* out) const {
  common::file_system::ascii_directory_string_path ps = GetHttpRoot();
  const std::string http_root_str = ps.GetPath();

  json_object_object_add(out, FIELD_OUTPUT_ID, json_object_new_int64(GetID()));
  std::string url_str = common::ConvertToString(GetOutput());
  json_object_object_add(out, FIELD_OUTPUT_URI, json_object_new_string(url_str.c_str()));
  json_object_object_add(out, FIELD_OUTPUT_HTTP_ROOT, json_object_new_string(http_root_str.c_str()));
  return common::Error();
}

bool IsTestOutputUrl(const OutputUri& url) {
  return url.GetOutput() == common::uri::Url(TEST_URL);
}

}  // namespace iptv_cloud
