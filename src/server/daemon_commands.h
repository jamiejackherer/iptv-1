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

#include "protocol/types.h"

// daemon
// client commands

#define CLIENT_START_STREAM "start_stream"  // {"config": {...}, "command_line": {...} }
#define CLIENT_STOP_STREAM "stop_stream"
#define CLIENT_RESTART_STREAM "restart_stream"
#define CLIENT_GET_LOG_STREAM "get_log_stream"
#define CLIENT_GET_PIPELINE_STREAM "get_pipeline_stream"

#define CLIENT_ACTIVATE "activate_request"  // {"key": "XXXXXXXXXXXXXXXXXX"}
#define CLIENT_STOP_SERVICE "stop_service"  // {"delay": 0 }
#define CLIENT_PREPARE_SERVICE \
  "prepare_service"  // { "feedback_directory": "", "timeshifts_directory": "", "hls_directory": "",
                     // "playlists_directory": "", "dvb_directory": "", "capture_card_directory": "" }
#define CLIENT_SYNC_SERVICE "sync_service"
#define CLIENT_PING_SERVICE "ping_service"
#define CLIENT_GET_LOG_SERVICE "get_log_service"  // {"path":"http://localhost/service/id"}

#define SERVER_PING "ping_client"

// Broadcast
#define CLIENT_CHANGED_SOURCES_STREAM "changed_source_stream"
#define CLIENT_STATISTIC_STREAM "statistic_stream"
#define CLIENT_QUIT_STATUS_STREAM "quit_status_stream"
#define CLIENT_STATISTIC_SERVICE "statistic_service"

namespace iptv_cloud {
namespace server {

// requests
protocol::request_t StopServiceRequest(protocol::sequance_id_t id,
                                       protocol::serializet_params_t params);  // StopInfo
protocol::request_t PingDaemonRequest(protocol::sequance_id_t id,
                                      protocol::serializet_params_t params);  // ServerPingInfo

// responces service
protocol::response_t StopServiceResponceSuccess(protocol::sequance_id_t id);
protocol::response_t StopServiceResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t GetLogServiceResponceSuccess(protocol::sequance_id_t id);
protocol::response_t GetLogServiceResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t ActivateResponce(protocol::sequance_id_t id, const std::string& result);  // ServerInfo
protocol::response_t ActivateResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t StateServiceResponce(protocol::sequance_id_t id, const std::string& result);  // Directories

protocol::response_t SyncServiceResponceSuccess(protocol::sequance_id_t id);

protocol::response_t PingServiceResponce(protocol::sequance_id_t id,
                                         const std::string& result);  // ServerPingInfo
protocol::response_t PingServiceResponceFail(protocol::sequance_id_t id, const std::string& error_text);

// responces streams
protocol::response_t StartStreamResponceSuccess(protocol::sequance_id_t id);
protocol::response_t StartStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t StopStreamResponceSuccess(protocol::sequance_id_t id);
protocol::response_t StopStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t RestartStreamResponceSuccess(protocol::sequance_id_t id);
protocol::response_t RestartStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text);

protocol::response_t GetLogStreamResponceSuccess(protocol::sequance_id_t id);
protocol::response_t GetLogStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text);

// Broadcast
protocol::request_t ChangedSourcesStreamBroadcast(protocol::serializet_params_t params);  // ChangedSouresInfo
protocol::request_t StatisitcStreamBroadcast(protocol::serializet_params_t params);       // StatisticInfo
protocol::request_t StatisitcServiceBroadcast(protocol::serializet_params_t params);      // ServerInfo
protocol::request_t QuitStatusStreamBroadcast(protocol::serializet_params_t params);      // StatusInfo

}  // namespace server
}  // namespace iptv_cloud
