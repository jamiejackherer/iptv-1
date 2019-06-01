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

#include <common/draw/types.h>
#include <common/uri/url.h>

#include "base/types.h"

namespace iptv_cloud {

class Logo {
 public:
  Logo();
  Logo(const common::uri::Url& path, const common::draw::Point& position, alpha_t alpha);

  bool IsValid() const;

  bool Equals(const Logo& logo) const;

  common::uri::Url GetPath() const;
  void SetPath(const common::uri::Url& path);

  common::draw::Point GetPosition() const;
  void SetPosition(const common::draw::Point& position);

  alpha_t GetAlpha() const;
  void SetAlpha(alpha_t alpha);

 private:
  common::uri::Url path_;
  common::draw::Point position_;
  alpha_t alpha_;
};

inline bool operator==(const Logo& left, const Logo& right) {
  return left.Equals(right);
}

inline bool operator!=(const Logo& left, const Logo& right) {
  return !operator==(left, right);
}

}  // namespace iptv_cloud

namespace common {
std::string ConvertToString(const iptv_cloud::Logo& value);  // json
bool ConvertFromString(const std::string& from, iptv_cloud::Logo* out);
}  // namespace common
