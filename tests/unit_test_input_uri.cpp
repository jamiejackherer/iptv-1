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

#include <json-c/json_object.h>

#include "base/input_uri.h"

#define RTMP_INPUT "rtmp://4.31.30.153:1935/devapp/tokengenffmpeg1"
#define FILE_INPUT "file:///home/sasha/2.txt"
#define DEVICE_VIDEO "/dev/video3"
#define DEVICE_AUDIO "audio=hw:3,0"
#define DEVICE_INPUT "dev://" DEVICE_VIDEO "?" DEVICE_AUDIO

TEST(InputUri, ConvertFromString) {
  const std::string invalid_uri_json =
      "{ \"id\": 0, \"uri\": \"\", \"mute\": false, \"relay_video\": false, \"relay_audio\": false }";
  iptv_cloud::InputUri invalid_uri;
  ASSERT_EQ(invalid_uri.GetID(), 0);
  ASSERT_EQ(invalid_uri.GetInput(), common::uri::Url());
  std::string conv;
  common::Error err = invalid_uri.SerializeToString(&conv);
  ASSERT_FALSE(err);
  ASSERT_EQ(conv, invalid_uri_json);

  const std::string uri_json = "{ \"id\": 1, \"uri\": \"" RTMP_INPUT
                               "\", \"volume\": 1.00, \"mute\": false, \"relay_video\": true, \"relay_audio\": false}";
  iptv_cloud::InputUri uri;
  err = uri.DeSerializeFromString(uri_json);
  ASSERT_FALSE(err);
  ASSERT_EQ(uri.GetID(), 1);
  common::uri::Url ro(RTMP_INPUT);
  ASSERT_EQ(uri.GetInput(), ro);
  ASSERT_EQ(uri.GetMute(), false);
  ASSERT_EQ(uri.GetRelayVideo(), true);
  ASSERT_EQ(uri.GetRelayAudio(), false);

  const std::string file_uri_json =
      "{ \"id\": 2, \"uri\": \"" FILE_INPUT
      "\", \"volume\": 1.0, \"mute\": false, \"relay_video\": false, \"relay_audio\": true}";
  iptv_cloud::InputUri file_uri;
  err = file_uri.DeSerializeFromString(file_uri_json);
  ASSERT_FALSE(err);
  ASSERT_EQ(file_uri.GetID(), 2);
  common::uri::Url file_ro(FILE_INPUT);
  ASSERT_EQ(file_uri.GetInput(), file_ro);
  ASSERT_EQ(file_uri.GetMute(), false);
  ASSERT_EQ(file_uri.GetRelayVideo(), false);
  ASSERT_EQ(file_uri.GetRelayAudio(), true);

  const std::string dev_uri_json =
      "{ \"id\": 2, \"uri\": \"" DEVICE_INPUT
      "\", \"volume\": 1.00, \"mute\": true, \"relay_video\": true, \"relay_audio\": true}";
  iptv_cloud::InputUri dev_uri;
  err = dev_uri.DeSerializeFromString(dev_uri_json);
  ASSERT_FALSE(err);
  ASSERT_EQ(dev_uri.GetID(), 2);
  common::uri::Url dev_ro(DEVICE_INPUT);
  ASSERT_EQ(dev_uri.GetInput(), dev_ro);
  ASSERT_TRUE(dev_ro.GetScheme() == common::uri::Url::dev);
  common::uri::Upath dpath = dev_ro.GetPath();
  ASSERT_EQ(dpath.GetPath(), DEVICE_VIDEO);
  ASSERT_EQ(dpath.GetQuery(), DEVICE_AUDIO);
  ASSERT_EQ(dev_uri.GetMute(), true);
  ASSERT_EQ(dev_uri.GetRelayVideo(), true);
  ASSERT_EQ(dev_uri.GetRelayAudio(), true);
}
