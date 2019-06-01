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

#include "stream/streams/vod/vod_encoding_stream.h"

#include "utils/utils.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

VodEncodeStream::VodEncodeStream(const VodEncodeConfig* config, IStreamClient* client, StreamStruct* stats)
    : base_class(config, client, stats) {
  CHECK(config->IsVod());
}

const char* VodEncodeStream::ClassName() const {
  return "VodEncodeStream";
}

void VodEncodeStream::PostExecCleanup() {
  const VodEncodeConfig* vconfig = static_cast<const VodEncodeConfig*>(GetConfig());
  if (vconfig->GetCleanupTS()) {
    for (const OutputUri& output : vconfig->GetOutput()) {
      common::uri::Url uri = output.GetOutput();
      common::uri::Url::scheme scheme = uri.GetScheme();

      if (scheme == common::uri::Url::http) {
        const common::file_system::ascii_directory_string_path http_path = output.GetHttpRoot();
        utils::RemoveFilesByExtension(http_path, CHUNK_EXT);
      }
    }
  }
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
