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

#include "base/config_fields.h"
#include "base/constants.h"
#include "base/inputs_outputs.h"

#include "stream/configs_factory.h"
#include "stream/stypes.h"

TEST(Api, init) {
  iptv_cloud::utils::ArgsMap emp;
  iptv_cloud::stream::Config* empty_api = nullptr;
  common::Error err = iptv_cloud::stream::make_config(emp, &empty_api);
  ASSERT_TRUE(err);
  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri uri;
  uri.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri);

  ASSERT_TRUE(iptv_cloud::IsTestInputUrl(iptv_cloud::InputUri(0, common::uri::Url(TEST_URL))));

  ASSERT_TRUE(iptv_cloud::stream::IsScreenUrl(common::uri::Url(SCREEN_URL)));
  ASSERT_TRUE(iptv_cloud::stream::IsRecordingUrl(common::uri::Url(RECORDING_URL)));
}

TEST(Api, logo) {
  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri uri2;
  uri2.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri2);
}
