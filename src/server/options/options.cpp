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

#include "server/options/options.h"

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>

#include <common/file_system/file_system.h>

#include "base/config_fields.h"
#include "base/constants.h"
#include "base/gst_constants.h"
#include "base/inputs_outputs.h"
#include "base/logo.h"

#include "utils/arg_converter.h"

namespace iptv_cloud {
namespace server {
namespace options {

namespace {

Validity dont_validate(const std::string&) {
  return Validity::VALID;
}

template <typename T>
Validity validate_range(const std::string& value, T min, T max, bool is_fatal) {
  T i;
  if (common::ConvertFromString(value, &i) && i >= min && i <= max) {
    return Validity::VALID;
  }

  return is_fatal ? Validity::FATAL : Validity::INVALID;
}

Validity validate_is_positive(const std::string& value, bool is_fatal) {
  int64_t i;
  if (common::ConvertFromString(value, &i) && i >= 0) {
    return Validity::VALID;
  }

  return is_fatal ? Validity::FATAL : Validity::INVALID;
}

Validity validate_id(const std::string& value) {
  return value.empty() ? Validity::INVALID : Validity::VALID;
}

Validity validate_input(const std::string& value) {
  UNUSED(value);
  return Validity::VALID;
}

Validity validate_output(const std::string& value) {
  iptv_cloud::output_t output;
  if (!common::ConvertFromString(value, &output)) {
    return Validity::INVALID;
  }
  return Validity::VALID;
}

Validity validate_restart_attempts(const std::string& value) {
  return validate_range<size_t>(value, 1, std::numeric_limits<size_t>::max(), false);
}

Validity validate_feedback_dir(const std::string& value) {
  return common::file_system::is_valid_path(value) ? Validity::VALID : Validity::INVALID;
}

Validity validate_timeshift_dir(const std::string& value) {
  return common::file_system::is_valid_path(value) ? Validity::VALID : Validity::FATAL;
}

Validity validate_timeshift_chunk_life_time(const std::string& value) {
  return validate_range(value, 0, 12 * 24 * 3600, false);
}

Validity validate_timeshift_delay(const std::string& value) {
  return validate_range(value, 0, 12 * 24 * 3600, false);
}

Validity validate_video_parser(const std::string& value) {
  for (size_t i = 0; i < SUPPORTED_VIDEO_PARSERS_COUNT; ++i) {
    const char* parser = kSupportedVideoParsers[i];
    if (value == parser) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_audio_parser(const std::string& value) {
  for (size_t i = 0; i < SUPPORTED_AUDIO_PARSERS_COUNT; ++i) {
    const char* parser = kSupportedAudioParsers[i];
    if (value == parser) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_video_codec(const std::string& value) {
  for (size_t i = 0; i < SUPPORTED_VIDEO_ENCODERS_COUNT; ++i) {
    const char* codec = kSupportedVideoEncoders[i];
    if (value == codec) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_audio_codec(const std::string& value) {
  for (size_t i = 0; i < SUPPORTED_AUDIO_ENCODERS_COUNT; ++i) {
    const char* codec = kSupportedAudioEncoders[i];
    if (value == codec) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_type(const std::string& value) {
  return validate_range<uint8_t>(value, iptv_cloud::StreamType::RELAY, iptv_cloud::StreamType::SCREEN, false);
}

Validity validate_log_level(const std::string& value) {
  return validate_range(value, static_cast<int>(common::logging::LOG_LEVEL_EMERG),
                        static_cast<int>(common::logging::LOG_LEVEL_DEBUG), false);
}

Validity validate_volume(const std::string& value) {
  return validate_range(value, 0.0, 10.0, false);
}

Validity validate_delay_time(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_timeshift_chunk_duration(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_auto_exit_time(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_size(const std::string& value) {
  common::draw::Size size;
  return common::ConvertFromString(value, &size) ? Validity::VALID : Validity::INVALID;
}

Validity validate_cleanupts(const std::string& value) {
  bool cleanup;
  return common::ConvertFromString(value, &cleanup) ? Validity::VALID : Validity::INVALID;
}

Validity validate_logo(const std::string& value) {
  Logo logo;
  return common::ConvertFromString(value, &logo) ? Validity::VALID : Validity::INVALID;
}

Validity validate_framerate(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_aspect_ratio(const std::string& value) {
  common::media::Rational rat;
  return common::ConvertFromString(value, &rat) ? Validity::VALID : Validity::INVALID;
}

Validity validate_decklink_video_mode(const std::string& value) {
  return validate_range(value, 0, 30, false);
}

Validity validate_video_bitrate(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_bitrate(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_channels(const std::string& value) {
  return validate_is_positive(value, false);
}

Validity validate_audio_select(const std::string& value) {
  int ais;
  return common::ConvertFromString(value, &ais) ? Validity::VALID : Validity::INVALID;
}

Validity validate_mfxh264_preset(const std::string& value) {
  return validate_range(value, 0, 7, false);
}

Validity validate_mfxh264_gopsize(const std::string& value) {
  return validate_range(value, 0, 65535, false);
}

Validity validate_nvh264_preset(const std::string& value) {
  return validate_range<uint16_t>(value, 0, std::numeric_limits<uint16_t>::max(), false);
}

// x264enc validators

Validity validate_x264_speed_preset(const std::string& value) {
  return validate_range(value, 0, 10, false);
}

Validity validate_x264_threads(const std::string& value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

Validity validate_x264_tune(const std::string& value) {
  static const int allowed_values[] = {0x0, 0x1, 0x2, 0x4};
  int32_t tune;
  if (!common::ConvertFromString(value, &tune)) {
    return Validity::INVALID;
  }

  for (auto i : allowed_values) {
    if (i == tune) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_x264_key_int_max(const std::string& value) {
  return validate_range(value, 0, std::numeric_limits<int>::max(), false);
}

Validity validate_x264_vbv_buf_capacity(const std::string& value) {
  return validate_range(value, 0, 10000, false);
}

Validity validate_x264_rc_lookahead(const std::string& value) {
  return validate_range(value, 0, 250, false);
}

Validity validate_x264_qp_max(const std::string& value) {
  return validate_range(value, 0, 51, false);
}

Validity validate_x264_pass(const std::string& value) {
  static const int allowed_values[] = {0, 4, 5, 17, 18, 19};
  int32_t pass;
  if (!common::ConvertFromString(value, &pass)) {
    return Validity::INVALID;
  }

  for (const auto& i : allowed_values) {
    if (i == pass) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_x264_me(const std::string& value) {
  return validate_range(value, 0, 4, false);
}

// vaapih264 validators

Validity validate_vaapih264_keyframe_period(const std::string& value) {
  return validate_range(value, 0, 300, false);
}

Validity validate_vaapih264_tune(const std::string& value) {
  static const int allowed_values[] = {0, 1, 3};
  int32_t tune;
  if (!common::ConvertFromString(value, &tune)) {
    return Validity::INVALID;
  }
  for (const auto& i : allowed_values) {
    if (i == tune) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_vaapih264_max_bframes(const std::string& value) {
  return validate_range(value, 0, 10, false);
}

Validity validate_vaapih264_num_slices(const std::string& value) {
  return validate_range(value, 1, 200, false);
}

Validity validate_vaapih264_init_qp(const std::string& value) {
  return validate_range(value, 1, 51, false);
}

Validity validate_vaapih264_min_qp(const std::string& value) {
  return validate_range(value, 1, 51, false);
}

Validity validate_vaapih264_rate_control(const std::string& value) {
  static const int allowed_values[] = {1, 2, 4, 5};
  int32_t rate_control;
  if (!common::ConvertFromString(value, &rate_control)) {
    return Validity::INVALID;
  }

  for (const auto& i : allowed_values) {
    if (i == rate_control) {
      return Validity::VALID;
    }
  }
  return Validity::INVALID;
}

Validity validate_vaapih264_cpb_length(const std::string& value) {
  return validate_range(value, 0, 10000, false);
}

Validity dummy_validator_integer(const std::string& value) {
  return validate_range(value, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
}

Validity dummy_validator_string(const std::string& value) {
  return value.empty() ? Validity::INVALID : Validity::VALID;
}

class ConstantOptions : public std::vector<option_t> {
 public:
  ConstantOptions(std::initializer_list<option_t> l) {
    for (auto it = l.begin(); it != l.end(); ++it) {
      option_t opt = *it;
      for (auto jt = begin(); jt != end(); ++jt) {
        option_t opt2 = *jt;
        if (opt2.first == opt.first) {
          NOTREACHED() << "Only unique options, but option with name: \"" << opt.first << "\" exists!";
        }
      }
      push_back(opt);
    }
  }
};
}  // namespace

bool FindCmdOption(const std::string& key, option_t* opt) {
  if (!opt) {
    return false;
  }

  static const ConstantOptions ALLOWED_CMD_OPTIONS = {};
  for (const option_t& cur : ALLOWED_CMD_OPTIONS) {
    if (cur.first == key) {
      *opt = cur;
      return true;
    }
  }

  return false;
}

utils::ArgsMap ValidateConfig(const std::string& full_config) {
  if (full_config.empty()) {
    CRITICAL_LOG() << "Invalid config data!";
  }

  json_object* obj = json_tokener_parse(full_config.c_str());
  if (!obj) {
    CRITICAL_LOG() << "Invalid config data!";
  }

  utils::ArgsMap options;
  json_object_object_foreach(obj, key, val) {
    option_t option;
    if (!FindOption(key, &option)) {
      WARNING_LOG() << "Unknown option: " << key;
    } else {
      const std::string value_str = val ? json_object_get_string(val) : "null";
      switch (option.second(value_str)) {
        case Validity::VALID:
          options[key] = value_str;
          break;
        case Validity::INVALID:
          WARNING_LOG() << "Invalid value \"" << value_str << "\" of option " << key;
          break;
        case Validity::FATAL:
          CRITICAL_LOG() << "Invalid value \"" << value_str << "\" of option " << key;
          break;
        default:
          NOTREACHED();
          break;
      }
    }
  }

  json_object_put(obj);
  return options;
}

bool FindOption(const std::string& key, option_t* opt) {
  if (!opt) {
    return false;
  }

  static const ConstantOptions ALLOWED_OPTIONS = {{ID_FIELD, validate_id},
                                                  {TYPE_FIELD, validate_type},
                                                  {FEEDBACK_DIR_FIELD, validate_feedback_dir},
                                                  {LOG_LEVEL_FIELD, validate_log_level},
                                                  {INPUT_FIELD, validate_input},
                                                  {OUTPUT_FIELD, validate_output},
                                                  {RESTART_ATTEMPTS_FIELD, validate_restart_attempts},
                                                  {AUTO_EXIT_TIME_FIELD, validate_auto_exit_time},
                                                  {TIMESHIFT_DIR_FIELD, validate_timeshift_dir},
                                                  {TIMESHIFT_CHUNK_LIFE_TIME_FIELD, validate_timeshift_chunk_life_time},
                                                  {TIMESHIFT_DELAY_FIELD, validate_timeshift_delay},
                                                  {MAIN_PROFILE_FIELD, dont_validate},
                                                  {MAIN_PROFILE_EXTERNAL_FIELD, dont_validate},
                                                  {VOLUME_FIELD, validate_volume},
                                                  {DELAY_TIME_FIELD, validate_delay_time},
                                                  {TIMESHIFT_CHUNK_DURATION_FIELD, validate_timeshift_chunk_duration},
                                                  {VIDEO_PARSER_FIELD, validate_video_parser},
                                                  {AUDIO_PARSER_FIELD, validate_audio_parser},
                                                  {AUDIO_CODEC_FIELD, validate_audio_codec},
                                                  {VIDEO_CODEC_FIELD, validate_video_codec},
                                                  {HAVE_VIDEO_FIELD, dont_validate},
                                                  {HAVE_AUDIO_FIELD, dont_validate},
                                                  {DEINTERLACE_FIELD, dont_validate},
                                                  {RELAY_AUDIO_FIELD, dont_validate},
                                                  {RELAY_VIDEO_FIELD, dont_validate},
                                                  {LOOP_FIELD, dont_validate},
                                                  {AVFORMAT_FIELD, dont_validate},
                                                  {SIZE_FIELD, validate_size},
                                                  {CLEANUP_TS_FIELD, validate_cleanupts},
                                                  {LOGO_FIELD, validate_logo},
                                                  {FRAME_RATE_FIELD, validate_framerate},
                                                  {ASPECT_RATIO_FIELD, validate_aspect_ratio},
                                                  {VIDEO_BIT_RATE_FIELD, validate_video_bitrate},
                                                  {AUDIO_BIT_RATE_FIELD, validate_audio_bitrate},
                                                  {AUDIO_CHANNELS_FIELD, validate_audio_channels},
                                                  {AUDIO_SELECT_FIELD, validate_audio_select},
                                                  {DECKLINK_VIDEO_MODE_FILELD, validate_decklink_video_mode},
                                                  {NV_H264_ENC_PRESET, validate_nvh264_preset},
                                                  {MFX_H264_ENC_PRESET, validate_mfxh264_preset},
                                                  {MFX_H264_GOP_SIZE, validate_mfxh264_gopsize},
                                                  {X264_ENC_SPEED_PRESET, validate_x264_speed_preset},
                                                  {X264_ENC_THREADS, validate_x264_threads},
                                                  {X264_ENC_TUNE, validate_x264_tune},
                                                  {X264_ENC_KEY_INT_MAX, validate_x264_key_int_max},
                                                  {X264_ENC_VBV_BUF_CAPACITY, validate_x264_vbv_buf_capacity},
                                                  {X264_ENC_RC_LOOKAHED, validate_x264_rc_lookahead},
                                                  {X264_ENC_QP_MAX, validate_x264_qp_max},
                                                  {X264_ENC_PASS, validate_x264_pass},
                                                  {X264_ENC_ME, validate_x264_me},
                                                  {X264_ENC_PROFILE, dummy_validator_string},
                                                  {X264_ENC_STREAM_FORMAT, dummy_validator_string},
                                                  {X264_ENC_OPTION_STRING, dont_validate},
                                                  {X264_ENC_INTERLACED, dont_validate},
                                                  {X264_ENC_DCT8X8, dont_validate},
                                                  {X264_ENC_B_ADAPT, dont_validate},
                                                  {X264_ENC_BYTE_STREAM, dont_validate},
                                                  {X264_ENC_CABAC, dont_validate},
                                                  {X264_ENC_SLICED_THREADS, dont_validate},
                                                  {X264_ENC_QUANTIZER, dont_validate},
                                                  {VAAPI_H264_ENC_KEYFRAME_PERIOD, validate_vaapih264_keyframe_period},
                                                  {VAAPI_H264_ENC_TUNE, validate_vaapih264_tune},
                                                  {VAAPI_H264_ENC_MAX_BFRAMES, validate_vaapih264_max_bframes},
                                                  {VAAPI_H264_ENC_NUM_SLICES, validate_vaapih264_num_slices},
                                                  {VAAPI_H264_ENC_INIT_QP, validate_vaapih264_init_qp},
                                                  {VAAPI_H264_ENC_MIN_QP, validate_vaapih264_min_qp},
                                                  {VAAPI_H264_ENC_RATE_CONTROL, validate_vaapih264_rate_control},
                                                  {VAAPI_H264_ENC_CABAC, dont_validate},
                                                  {VAAPI_H264_ENC_DCT8X8, dont_validate},
                                                  {VAAPI_H264_ENC_CPB_LENGTH, validate_vaapih264_cpb_length},
                                                  {OPEN_H264_ENC_MUTLITHREAD, dont_validate},
                                                  {OPEN_H264_ENC_COMPLEXITY, dummy_validator_integer},
                                                  {OPEN_H264_ENC_RATE_CONTROL, dummy_validator_integer},
                                                  {OPEN_H264_ENC_GOP_SIZE, dummy_validator_integer},
                                                  {EAVC_ENC_PRESET, dummy_validator_integer},
                                                  {EAVC_ENC_PROFILE, dummy_validator_integer},
                                                  {EAVC_ENC_PERFORMANCE, dummy_validator_integer},
                                                  {EAVC_ENC_BITRATE_MODE, dummy_validator_integer},
                                                  {EAVC_ENC_BITRATE_PASS, dummy_validator_integer},
                                                  {EAVC_ENC_BITRATE_MAX, dummy_validator_integer},
                                                  {EAVC_ENC_VBV_SIZE, dummy_validator_integer},
                                                  {EAVC_ENC_PICTURE_MODE, dummy_validator_integer},
                                                  {EAVC_ENC_ENTROPY_MODE, dummy_validator_integer},
                                                  {EAVC_ENC_GOP_MAX_BCOUNT, dummy_validator_integer},
                                                  {EAVC_ENC_GOP_MAX_LENGTH, dummy_validator_integer},
                                                  {EAVC_ENC_GOP_MIN_LENGTH, dummy_validator_integer},
                                                  {EAVC_ENC_LEVEL, dummy_validator_integer},
                                                  {EAVC_ENC_DEBLOCK_MODE, dummy_validator_integer},
                                                  {EAVC_ENC_DEBLOCK_ALPHA, dummy_validator_integer},
                                                  {EAVC_ENC_DEBLOCK_BETA, dummy_validator_integer},
                                                  {EAVC_ENC_INITIAL_DELAY, dummy_validator_integer},
                                                  {EAVC_ENC_FIELD_ORDER, dummy_validator_integer},
                                                  {EAVC_ENC_GOP_ADAPTIVE, dont_validate}};
  for (const option_t& cur : ALLOWED_OPTIONS) {
    if (cur.first == key) {
      *opt = cur;
      return true;
    }
  }

  return false;
}

}  // namespace options
}  // namespace server
}  // namespace iptv_cloud
