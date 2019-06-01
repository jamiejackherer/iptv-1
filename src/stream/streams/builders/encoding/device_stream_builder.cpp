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

#include "stream/streams/builders/encoding/device_stream_builder.h"

#include <common/sprintf.h>

#include "base/input_uri.h"  // for InputUri

#include "stream/pad/pad.h"  // for Pad

#include "stream/elements/element.h"  // for Element
#include "stream/elements/sources/alsasrc.h"
#include "stream/elements/sources/v4l2src.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

DeviceStreamBuilder::DeviceStreamBuilder(const EncodingConfig* api, SrcDecodeBinStream* observer)
    : EncodingStreamBuilder(api, observer) {}

Connector DeviceStreamBuilder::BuildInput() {
  const EncodingConfig* config = static_cast<const EncodingConfig*>(GetConfig());
  input_t input = config->GetInput();
  InputUri diuri = input[0];
  common::uri::Url duri = diuri.GetInput();
  common::uri::Upath dpath = duri.GetPath();
  elements::Element* video = nullptr;
  if (config->HaveVideo()) {
    video = elements::sources::make_v4l2_src(dpath.GetPath(), 0);
    ElementAdd(video);
    pad::Pad* src_pad = video->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(duri.GetScheme(), src_pad, 0);
    }
    delete src_pad;

    elements::ElementCapsFilter* capsfilter =
        new elements::ElementCapsFilter(common::MemSPrintf(VIDEO_CAPS_DEVICE_NAME_1U, 0));
    ElementAdd(capsfilter);

    GstCaps* cap_width_height = gst_caps_new_simple("video/x-raw", "width", G_TYPE_INT, VIDEO_WIDTH, "height",
                                                    G_TYPE_INT, VIDEO_HEIGHT, nullptr);
    capsfilter->SetCaps(cap_width_height);
    gst_caps_unref(cap_width_height);

    ElementLink(video, capsfilter);
    video = capsfilter;
  }

  elements::Element* audio = nullptr;
  if (config->HaveAudio()) {
    audio = elements::sources::make_alsa_src(dpath.GetQuery(), 1);
    ElementAdd(audio);
    pad::Pad* src_pad = audio->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(duri.GetScheme(), src_pad, 1);
    }
    delete src_pad;
  }
  return {video, audio};
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
