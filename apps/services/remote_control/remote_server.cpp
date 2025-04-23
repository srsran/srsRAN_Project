/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "apps/services/remote_control/remote_server.h"
#include "apps/services/remote_control/remote_command.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "nlohmann/json.hpp"
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#include "uWebSockets/App.h"
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
#include "srsran/support/executors/unique_thread.h"
#include <csignal>

using namespace srsran;
using namespace app_services;

/// Builds an error response encoded in JSON format.
static std::string build_error_response(std::string_view error, std::optional<std::string> cmd = std::nullopt)
{
  nlohmann::json response;
  response["error"] = error;
  if (cmd) {
    response["cmd"] = std::move(*cmd);
  }
  return response.dump();
}

/// Builds a successful response encoded in JSON format.
static std::string build_success_response(std::string_view cmd)
{
  nlohmann::json response;
  response["cmd"] = cmd;
  return response.dump();
}

namespace {

/// Command that manages closing the application.
class quit_remote_command : public remote_command
{
public:
  // See interface for documentation.
  std::string_view get_name() const override { return "quit"; }

  // See interface for documentation.
  std::string_view get_description() const override { return "Quit application"; }

  // See interface for documentation.
  error_type<std::string> execute(const nlohmann::json& json) override
  {
    std::raise(SIGTERM);
    return {};
  }
};

/// Remote server implementation.
class remote_server_impl : public remote_server
{
public:
  remote_server_impl(const std::string& bind_addr, unsigned port, span<std::unique_ptr<remote_command>> commands_)
  {
    // Add the quit command.
    {
      auto  quit_cmd = std::make_unique<quit_remote_command>();
      auto& cmd      = commands[std::string(quit_cmd->get_name())];
      cmd            = std::move(quit_cmd);
    }

    // Store the remote commands.
    for (auto& remote_cmd : commands_) {
      auto& cmd = commands[std::string(remote_cmd->get_name())];
      cmd       = std::move(remote_cmd);
    }

    thread = unique_thread("ws_server", [this, bind_addr, port]() {
      uWS::App ws_server;
      struct dummy_type {};
      ws_server
          .ws<dummy_type>("/*",
                          {.compression              = uWS::CompressOptions(uWS::DISABLED),
                           .maxPayloadLength         = 16 * 1024,
                           .idleTimeout              = 120,
                           .maxBackpressure          = 16 * 1024 * 1024,
                           .closeOnBackpressureLimit = false,
                           .resetIdleTimeoutOnSend   = false,
                           .sendPingsAutomatically   = true,
                           .upgrade                  = nullptr,
                           .message =
                               [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
                                 // Only parse text based messages.
                                 if (opCode != uWS::OpCode::TEXT) {
                                   ws->send(
                                       build_error_response("This WebSocket server only supports opcode TEXT messages"),
                                       uWS::OpCode::TEXT,
                                       false);
                                   return;
                                 }

                                 // Handle the incoming message and return back the response.
                                 std::string response = handle_command(message);
                                 ws->send(response, uWS::OpCode::TEXT, false);
                               }})
          .listen(bind_addr, port, [bind_addr, port](auto* listen_socket) {
            if (listen_socket) {
              fmt::println("Remote control server listening on {}:{}", bind_addr, port);
            } else {
              fmt::println("Remote control server cannot listen on {}:{}", bind_addr, port);
            }
          });

      server      = &ws_server;
      server_loop = uWS::Loop::get();
      ws_server.run();
    });
  }

  // See interface for documentation.
  ~remote_server_impl() override { stop(); }

  // See interface for documentation.
  void stop() override
  {
    // Wait for completion.
    if (thread.running()) {
      server_loop.load()->defer([this]() { server->close(); });
      thread.join();
    }
  }

  /// Handles the given command.
  std::string handle_command(std::string_view command)
  {
    nlohmann::json req;

    try {
      req = nlohmann::json::parse(command);
    } catch (const nlohmann::json::parse_error& e) {
      return build_error_response(e.what());
    }

    auto cmd_key = req.find("cmd");
    if (cmd_key == req.end()) {
      return build_error_response("'cmd' object is missing and it is mandatory");
    }

    if (!cmd_key->is_string()) {
      return build_error_response("'cmd' object value type should be a string");
    }

    const auto& cmd_value = cmd_key.value().get_ref<const nlohmann::json::string_t&>();
    if (auto cmd = commands.find(cmd_value); cmd != commands.end()) {
      if (auto response_str = cmd->second->execute(req); !response_str) {
        return build_error_response(response_str.error(), cmd_value);
      }

      return build_success_response(cmd_value);
    }

    return build_error_response(fmt::format("Unknown command type: {}", cmd_value), cmd_value);
  }

private:
  unique_thread                                                    thread;
  std::atomic<uWS::Loop*>                                          server_loop;
  uWS::App*                                                        server;
  std::unordered_map<std::string, std::unique_ptr<remote_command>> commands;
};

} // namespace

std::unique_ptr<remote_server>
srsran::app_services::create_remote_server(const remote_control_appconfig&       cfg,
                                           span<std::unique_ptr<remote_command>> commands)
{
  if (!cfg.enabled) {
    return nullptr;
  }
  return std::make_unique<remote_server_impl>(cfg.bind_addr, cfg.port, commands);
}
