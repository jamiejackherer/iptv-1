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

#include "base/input_uri.h"

#include <string>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "base/constants.h"

#define FIELD_INPUT_ID "id"
#define FIELD_INPUT_URI "uri"
#define FIELD_USER_AGENT "user_agent"

namespace iptv_cloud {

InputUri::InputUri() : InputUri(0, common::uri::Url()) {}

InputUri::InputUri(uri_id_t id, const common::uri::Url& input, user_agent_t ua)
    : base_class(), id_(id), input_(input), user_agent_(ua) {}

InputUri::uri_id_t InputUri::GetID() const {
  return id_;
}

void InputUri::SetID(uri_id_t id) {
  id_ = id;
}

common::uri::Url InputUri::GetInput() const {
  return input_;
}

void InputUri::SetInput(const common::uri::Url& uri) {
  input_ = uri;
}

InputUri::user_agent_t InputUri::GetUserAgent() const {
  return user_agent_;
}

void InputUri::SetUserAgent(user_agent_t agent) {
  user_agent_ = agent;
}

bool InputUri::Equals(const InputUri& inf) const {
  return id_ == inf.id_ && input_ == inf.input_;
}

common::Error InputUri::DoDeSerialize(json_object* serialized) {
  InputUri res;
  json_object* jid = nullptr;
  json_bool jid_exists = json_object_object_get_ex(serialized, FIELD_INPUT_ID, &jid);
  if (jid_exists) {
    res.SetID(json_object_get_int64(jid));
  }

  json_object* juri = nullptr;
  json_bool juri_exists = json_object_object_get_ex(serialized, FIELD_INPUT_URI, &juri);
  if (juri_exists) {
    res.SetInput(common::uri::Url(json_object_get_string(juri)));
  }

  json_object* juser_agent = nullptr;
  json_bool juser_agent_exists = json_object_object_get_ex(serialized, FIELD_USER_AGENT, &juser_agent);
  if (juser_agent_exists) {
    user_agent_t agent = static_cast<user_agent_t>(json_object_get_int(juser_agent));
    res.SetUserAgent(agent);
  }

  *this = res;
  return common::Error();
}

common::Error InputUri::SerializeFields(json_object* out) const {
  json_object_object_add(out, FIELD_INPUT_ID, json_object_new_int64(GetID()));
  std::string url_str = common::ConvertToString(GetInput());
  json_object_object_add(out, FIELD_INPUT_URI, json_object_new_string(url_str.c_str()));
  json_object_object_add(out, FIELD_USER_AGENT, json_object_new_int(user_agent_));

  return common::Error();
}

bool IsTestInputUrl(const InputUri& url) {
  return url.GetInput() == common::uri::Url(TEST_URL);
}

}  // namespace iptv_cloud
