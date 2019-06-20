/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

    This file is part of FastoTV.

    FastoTV is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoTV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoTV. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>  // for string

#include <fastotv/protocol/types.h>
#include <fastotv/types.h>

namespace iptv_cloud {
namespace server {
namespace subscribers {

// requests
fastotv::protocol::request_t PingRequest(fastotv::protocol::sequance_id_t id,
                                         fastotv::protocol::serializet_params_t params);

// responces
fastotv::protocol::response_t ActivateResponseSuccess(fastotv::protocol::sequance_id_t id);
fastotv::protocol::response_t ActivateResponseFail(fastotv::protocol::sequance_id_t id, const std::string& error_text);

// client ping
fastotv::protocol::response_t PingResponseSuccess(fastotv::protocol::sequance_id_t id,
                                                  fastotv::protocol::serializet_params_t params);

// server info
fastotv::protocol::response_t GetServerInfoResponceSuccsess(fastotv::protocol::sequance_id_t id,
                                                            fastotv::protocol::serializet_params_t params);
fastotv::protocol::response_t GetServerInfoResponceFail(fastotv::protocol::sequance_id_t id,
                                                        const std::string& error_text);

// channels
fastotv::protocol::response_t GetChannelsResponceSuccsess(fastotv::protocol::sequance_id_t id,
                                                          fastotv::protocol::serializet_params_t params);
fastotv::protocol::response_t GetChannelsResponceFail(fastotv::protocol::sequance_id_t id,
                                                      const std::string& error_text);

// runtimeChannel
fastotv::protocol::response_t GetRuntimeChannelInfoResponceSuccsess(fastotv::protocol::sequance_id_t id,
                                                                    fastotv::protocol::serializet_params_t params);

}  // namespace subscribers
}  // namespace server
}  // namespace iptv_cloud
