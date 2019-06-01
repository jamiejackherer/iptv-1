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

#include "gtest/gtest.h"

#include "base/constants.h"

#include "server/options/options.h"
#include "utils/arg_converter.h"

#define LOGO_FIELD "logo"

namespace {
const char kTimeshiftRecorderConfig[] = R"({
    "id" : "test_1",
    "input" : {"urls" : [ {"id" : 1, "uri" : "http://example.com/manager/fo/forward2.php?cid=14"} ]},
    "output" : {"urls" : [ {"id" : 80, "timeshift_dir" : "/var/www/html/live/14"} ]},
    "type" : 3
  })";
}

TEST(Options, logo_path) {
  std::string cfg = "{\"" LOGO_FIELD "\" : {\"path\": \"file:///home/user/logo.png\"}}";
  auto args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_EQ(args.size(), 1);

  cfg = "{\"" LOGO_FIELD "\" : {\"path\": \"http://home/user/logo.png\"}}";
  args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_EQ(args.size(), 1);
}

TEST(Options, cfgs) {
  auto args = iptv_cloud::server::options::ValidateConfig(kTimeshiftRecorderConfig);
  ASSERT_EQ(args.size(), 4);
}
