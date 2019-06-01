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

#include "base/logo.h"

#include "stream/streams/configs/audio_video_config.h"

#include "stream/stypes.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

class EncodingConfig : public AudioVideoConfig {
 public:
  typedef AudioVideoConfig base_class;
  explicit EncodingConfig(const base_class& config);

  bool GetRelayVideo() const;
  void SetRelayVideo(bool rv);

  bool GetRelayAudio() const;
  void SetRelayAudio(bool ra);

  volume_t GetVolume() const;  // encoding
  void SetVolume(volume_t volume);

  frame_rate_t GetFramerate() const;  // encoding
  void SetFrameRate(frame_rate_t rate);

  deinterlace_t GetDeinterlace() const;  // encoding
  void SetDeinterlace(deinterlace_t deinterlace);

  std::string GetVideoEncoder() const;  // encoding
  void SetVideoEncoder(const std::string& enc);

  std::string GetAudioEncoder() const;  // encoding
  void SetAudioEncoder(const std::string& enc);

  bool IsGpu() const;     // encoding
  bool IsMfxGpu() const;  // encoding

  audio_channels_count_t GetAudioChannelsCount() const;  // encoding
  void SetAudioChannelsCount(audio_channels_count_t channels);

  video_encoders_args_t GetVideoEncoderArgs() const;  // encoding
  void SetVideoEncoderArgs(const video_encoders_args_t& args);

  video_encoders_str_args_t GetVideoEncoderStrArgs() const;  // encoding
  void SetVideoEncoderStrArgs(const video_encoders_str_args_t& args);

  common::draw::Size GetSize() const;  // encoding
  void SetSize(common::draw::Size size);

  bit_rate_t GetVideoBitrate() const;  // encoding
  void SetVideoBitrate(bit_rate_t bitr);

  bit_rate_t GetAudioBitrate() const;  // encoding
  void SetAudioBitrate(bit_rate_t bitr);

  Logo GetLogo() const;  // encoding
  void SetLogo(const Logo& logo);

  rational_t GetAspectRatio() const;  // encoding
  void SetAspectRatio(rational_t rat);

  decklink_video_mode_t GetDecklinkMode() const;  // mosaic
  void SetDecklinkMode(decklink_video_mode_t decl);

 private:
  deinterlace_t deinterlace_;

  frame_rate_t frame_rate_;
  volume_t volume_;

  std::string video_encoder_;
  std::string audio_encoder_;

  audio_channels_count_t audio_channels_count_;

  video_encoders_args_t video_encoder_args_;
  video_encoders_str_args_t video_encoder_str_args_;

  common::draw::Size size_;
  bit_rate_t video_bit_rate_;
  bit_rate_t audio_bit_rate_;

  Logo logo_;

  decklink_video_mode_t decklink_video_mode_;
  rational_t aspect_ratio_;

  bool relay_video_;
  bool relay_audio_;
};

class VodEncodeConfig : public EncodingConfig {
 public:
  typedef EncodingConfig base_class;
  explicit VodEncodeConfig(const base_class& config);

  bool GetCleanupTS() const;
  void SetCleanupTS(bool cleanup);

 private:
  bool cleanup_ts_;
};

typedef EncodingConfig PlaylistEncodingConfig;

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
