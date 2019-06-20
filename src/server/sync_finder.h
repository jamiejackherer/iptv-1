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

#include <map>
#include <mutex>

#include "server/subscribers/isubscribe_finder.h"

namespace iptv_cloud {
namespace server {

class SyncFinder : public subscribers::ISubscribeFinder {
 public:
  typedef std::map<fastotv::login_t, subscribers::commands_info::UserInfo> users_t;

  SyncFinder();
  common::Error FindUser(const fastotv::commands_info::AuthInfo& user,
                         subscribers::commands_info::UserInfo* uinf) const override;
  void Clear();
  void AddUser(const subscribers::commands_info::UserInfo& user);

 private:
  users_t users_;
  mutable std::mutex users_mutex_;
};

}  // namespace server
}  // namespace iptv_cloud
