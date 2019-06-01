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

#include "base/logo.h"

#include <string>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/sprintf.h>

#include "base/constants.h"

#define LOGO_PATH_FIELD "path"
#define LOGO_POSITION_FIELD "position"
#define LOGO_ALPHA_FIELD "alpha"

namespace iptv_cloud {

Logo::Logo() : Logo(common::uri::Url(), common::draw::Point(), alpha_t()) {}

Logo::Logo(const common::uri::Url& path, const common::draw::Point& position, alpha_t alpha)
    : path_(path), position_(position), alpha_(alpha) {}

bool Logo::IsValid() const {
  return path_.IsValid();
}

bool Logo::Equals(const Logo& inf) const {
  return path_ == inf.path_;
}

common::uri::Url Logo::GetPath() const {
  return path_;
}

void Logo::SetPath(const common::uri::Url& path) {
  path_ = path;
}

common::draw::Point Logo::GetPosition() const {
  return position_;
}

void Logo::SetPosition(const common::draw::Point& position) {
  position_ = position;
}

alpha_t Logo::GetAlpha() const {
  return alpha_;
}

void Logo::SetAlpha(alpha_t alpha) {
  alpha_ = alpha;
}

}  // namespace iptv_cloud

namespace common {

std::string ConvertToString(const iptv_cloud::Logo& value) {
  return common::MemSPrintf(
      "{ \"" LOGO_PATH_FIELD "\": \"%s\",\"" LOGO_POSITION_FIELD "\": \"%s\",\"" LOGO_ALPHA_FIELD "\": %lf }",
      common::ConvertToString(value.GetPath()), common::ConvertToString(value.GetPosition()), value.GetAlpha());
}

bool ConvertFromString(const std::string& from, iptv_cloud::Logo* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(from.c_str());
  if (!obj) {
    return false;
  }

  iptv_cloud::Logo res;
  json_object* jpath = nullptr;
  json_bool jpath_exists = json_object_object_get_ex(obj, LOGO_PATH_FIELD, &jpath);
  if (jpath_exists) {
    res.SetPath(common::uri::Url(json_object_get_string(jpath)));
  }

  json_object* jposition = nullptr;
  json_bool jposition_exists = json_object_object_get_ex(obj, LOGO_POSITION_FIELD, &jposition);
  if (jposition_exists) {
    common::draw::Point pt;
    if (common::ConvertFromString(json_object_get_string(jposition), &pt)) {
      res.SetPosition(pt);
    }
  }

  json_object* jalpha = nullptr;
  json_bool jalpha_exists = json_object_object_get_ex(obj, LOGO_ALPHA_FIELD, &jalpha);
  if (jalpha_exists) {
    res.SetAlpha(json_object_get_double(jalpha));
  }

  json_object_put(obj);
  *out = res;
  return true;
}

}  // namespace common
