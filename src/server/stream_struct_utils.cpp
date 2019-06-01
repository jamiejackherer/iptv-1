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

#include "server/stream_struct_utils.h"

#include <sys/mman.h>

namespace iptv_cloud {
namespace server {

common::ErrnoError AllocSharedStreamStruct(const StreamInfo& sha, StreamStruct** stream) {
  StreamStruct* mem = static_cast<StreamStruct*>(
      mmap(nullptr, sizeof(StreamStruct), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  if (!mem) {
    return common::make_errno_error("Failed to allocate memory.", ENOMEM);
  }

  *stream = new (mem) StreamStruct(sha);
  return common::ErrnoError();
}

void FreeSharedStreamStruct(StreamStruct** data) {
  if (!data) {
    return;
  }

  StreamStruct* ldata = *data;
  if (!ldata) {
    return;
  }

  ldata->~StreamStruct();
  munmap(ldata, sizeof(StreamStruct));
  *data = nullptr;
}

}  // namespace server
}  // namespace iptv_cloud
