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

#include <gmock/gmock.h>

#include <gst/gst.h>

#include <thread>

#include "base/constants.h"

#include "stream/configs_factory.h"
#include "stream/streams/screen_stream.h"

using testing::_;

class FakeObserver : public iptv_cloud::stream::IBaseStream::IStreamClient {
 public:
  MOCK_METHOD2(OnStatusChanged, void(iptv_cloud::stream::IBaseStream*, iptv_cloud::StreamStatus));
  MOCK_METHOD1(OnPipelineEOS, void(iptv_cloud::stream::IBaseStream*));
  MOCK_METHOD1(OnTimeoutUpdated, void(iptv_cloud::stream::IBaseStream*));
  MOCK_METHOD2(OnSyncMessageReceived, void(iptv_cloud::stream::IBaseStream*, GstMessage*));
  MOCK_METHOD2(OnASyncMessageReceived, void(iptv_cloud::stream::IBaseStream*, GstMessage*));
  void OnInputChanged(const iptv_cloud::InputUri& uri) override { UNUSED(uri); }
  GstPadProbeInfo* OnCheckReveivedOutputData(iptv_cloud::stream::IBaseStream* job,
                                             iptv_cloud::stream::Probe* probe,
                                             GstPadProbeInfo* info) override {
    UNUSED(job);
    UNUSED(probe);
    return info;
  }
  GstPadProbeInfo* OnCheckReveivedData(iptv_cloud::stream::IBaseStream* job,
                                       iptv_cloud::stream::Probe* probe,
                                       GstPadProbeInfo* info) override {
    UNUSED(job);
    UNUSED(probe);
    return info;
  }
  MOCK_METHOD3(OnProbeEvent, void(iptv_cloud::stream::IBaseStream*, iptv_cloud::stream::Probe*, GstEvent*));
  MOCK_METHOD1(OnPipelineCreated, void(iptv_cloud::stream::IBaseStream*));
};

void* quit_job(iptv_cloud::stream::IBaseStream* job) {
  sleep(5);
  job->Quit(iptv_cloud::stream::EXIT_SELF);
  return NULL;
}

TEST(Job, Status) {
  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri uri;
  uri.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri);

  FakeObserver cl;
  iptv_cloud::stream::streams_init(0, NULL);
  iptv_cloud::StreamStruct st(iptv_cloud::StreamInfo{"screen", iptv_cloud::SCREEN, {}, {0}});
  iptv_cloud::stream::IBaseStream* job = new iptv_cloud::stream::streams::ScreenStream(nullptr, &cl, &st);
  std::thread th(&quit_job, job);
  EXPECT_CALL(cl, OnStatusChanged(job, _)).Times(4);
  EXPECT_CALL(cl, OnTimeoutUpdated(job)).Times(::testing::AnyNumber());
  EXPECT_CALL(cl, OnSyncMessageReceived(job, _)).Times(::testing::AnyNumber());
  EXPECT_CALL(cl, OnProbeEvent(job, _, _)).Times(::testing::AnyNumber());
  job->Exec();
  th.join();
  delete job;
}
