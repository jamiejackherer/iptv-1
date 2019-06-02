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

#include <common/protocols/json_rpc/protocol_client.h>

#include <common/text_decoders/compress_zlib_edcoder.h>

namespace iptv_cloud {
namespace protocol {

class IptvClientCompressor : public common::CompressZlibEDcoder {
 public:
  typedef common::CompressZlibEDcoder base_class;
  IptvClientCompressor() : base_class(false, common::CompressZlibEDcoder::GZIP_DEFLATE) {}
};

template <typename Client>
using ProtocolClient = common::protocols::json_rpc::ProtocolClient<Client, IptvClientCompressor>;

typedef ProtocolClient<common::libev::IoClient> protocol_client_t;

}  // namespace protocol
}  // namespace iptv_cloud
