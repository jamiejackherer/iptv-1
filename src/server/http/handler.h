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

#include <common/file_system/path.h>
#include <common/libev/io_loop_observer.h>

namespace iptv_cloud {
namespace server {

class HttpClient;
class IHttpRequestsObserver;

class HttpHandler : public common::libev::IoLoopObserver {
 public:
  enum { BUF_SIZE = 4096 };
  typedef common::file_system::ascii_directory_string_path http_directory_path_t;
  explicit HttpHandler(IHttpRequestsObserver* observer);

  void SetHttpRoot(const http_directory_path_t& http_root);

  void PreLooped(common::libev::IoLoop* server) override;

  void Accepted(common::libev::IoClient* client) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoClient* client) override;
  void Closed(common::libev::IoClient* client) override;
  void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;
#if LIBEV_CHILD_ENABLE
  void Accepted(common::libev::IoChild* child) override;
  void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  void ChildStatusChanged(common::libev::IoChild* child, int status) override;
#endif

  void DataReceived(common::libev::IoClient* client) override;
  void DataReadyToWrite(common::libev::IoClient* client) override;

  void PostLooped(common::libev::IoLoop* server) override;

 private:
  void ProcessReceived(HttpClient* hclient, const char* request, size_t req_len);

  http_directory_path_t http_root_;
  IHttpRequestsObserver* observer_;
};

}  // namespace server
}  // namespace iptv_cloud
