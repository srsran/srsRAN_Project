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
#include "srsran/srslog/srslog.h"
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#include "uWebSockets/App.h"
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
#include "apps/helpers/metrics/helpers.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/support/executors/unique_thread.h"
#include <csignal>
#include <utility>

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
  response["timestamp"] = app_helpers::get_time_stamp();
  return response.dump();
}

/// Builds a successful response encoded in JSON format.
static std::string build_success_response(std::string_view cmd)
{
  nlohmann::json response;
  response["cmd"]       = cmd;
  response["timestamp"] = app_helpers::get_time_stamp();
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

class remote_server_impl;
/// Receives the formatted JSON metrics from the metrics log channel.
class remote_server_sink : public srslog::sink
{
public:
  explicit remote_server_sink(std::unique_ptr<srslog::log_formatter> f) : srslog::sink(std::move(f)) {}

  /// Identifier of this custom sink.
  static const char* name() { return "remote_server_sink"; }

  // See interface for documentation.
  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override;

  // See interface for documentation.
  srslog::detail::error_string flush() override { return {}; }

  void set_server(remote_server_impl* server_)
  {
    std::scoped_lock lock(mutex);
    server = server_;
  }

private:
  remote_server_impl* server = nullptr;
  std::mutex          mutex;
};

/// Remote server implementation.
class remote_server_impl : public remote_server
{
  /// WebSocket socket type alias.
  struct dummy_type {};
  using socket_type = uWS::WebSocket<false, true, dummy_type>;

  unique_thread                                                    thread;
  std::atomic<uWS::App*>                                           server;
  std::atomic<uWS::Loop*>                                          server_loop;
  std::unordered_map<std::string, std::unique_ptr<remote_command>> commands;
  std::set<socket_type*>                                           metrics_subscribers;
  socket_type*                                                     current_cmd_client = nullptr;

  /// Metrics subscription command.
  class metrics_subscribe_command : public remote_command
  {
    remote_server_impl* parent;

  public:
    explicit metrics_subscribe_command(remote_server_impl* parent_) : parent(parent_) {}

    // See interface for documentation.
    std::string_view get_name() const override { return "metrics_subscribe"; }

    // See interface for documentation.
    std::string_view get_description() const override { return "Subscribe to metrics notifications"; }

    // See interface for documentation.
    error_type<std::string> execute(const nlohmann::json& json) override
    {
      parent->subscribe_metrics_client();
      return {};
    }
  };

  /// Metrics unsubscription command.
  class metrics_unsubscribe_command : public remote_command
  {
    remote_server_impl* parent;

  public:
    explicit metrics_unsubscribe_command(remote_server_impl* parent_) : parent(parent_) {}

    // See interface for documentation.
    std::string_view get_name() const override { return "metrics_unsubscribe"; }

    // See interface for documentation.
    std::string_view get_description() const override { return "Unsubscribe to metrics notifications"; }

    // See interface for documentation.
    error_type<std::string> execute(const nlohmann::json& json) override
    {
      parent->unsubscribe_metrics_client();
      return {};
    }
  };

public:
  remote_server_impl(const std::string&                    bind_addr,
                     unsigned                              port,
                     bool                                  enable_metrics_subscription,
                     span<std::unique_ptr<remote_command>> commands_)
  {
    // Add the quit command.
    {
      auto  quit_cmd = std::make_unique<quit_remote_command>();
      auto& cmd      = commands[std::string(quit_cmd->get_name())];
      cmd            = std::move(quit_cmd);
    }

    if (enable_metrics_subscription) {
      // Add the metrics subscription commands.
      {
        auto  sub_cmd = std::make_unique<metrics_subscribe_command>(this);
        auto& cmd     = commands[std::string(sub_cmd->get_name())];
        cmd           = std::move(sub_cmd);
      }
      {
        auto  unsub_cmd = std::make_unique<metrics_unsubscribe_command>(this);
        auto& cmd       = commands[std::string(unsub_cmd->get_name())];
        cmd             = std::move(unsub_cmd);
      }
    }

    // Store the remote commands.
    for (auto& remote_cmd : commands_) {
      auto& cmd = commands[std::string(remote_cmd->get_name())];
      cmd       = std::move(remote_cmd);
    }

    thread = unique_thread("ws_server", [this, bind_addr, port, enable_metrics_subscription]() {
      uWS::App ws_server;
      ws_server
          .ws<dummy_type>("/*",
                          {.compression              = uWS::CompressOptions(uWS::DISABLED),
                           .maxPayloadLength         = 16 * 1024,
                           .idleTimeout              = 120,
                           .maxBackpressure          = 16 * 1024 * 1024,
                           .closeOnBackpressureLimit = false,
                           .resetIdleTimeoutOnSend   = false,
                           .sendPingsAutomatically   = true,
                           .message =
                               [this](socket_type* ws, std::string_view message, uWS::OpCode opCode) {
                                 // Only parse text based messages.
                                 if (opCode != uWS::OpCode::TEXT) {
                                   ws->send(
                                       build_error_response("This WebSocket server only supports opcode TEXT messages"),
                                       uWS::OpCode::TEXT,
                                       false);
                                   return;
                                 }

                                 current_cmd_client  = ws;
                                 auto restore_client = make_scope_exit([this]() { current_cmd_client = nullptr; });

                                 // Handle the incoming message and return back the response.
                                 std::string response = handle_command(message);
                                 ws->send(response, uWS::OpCode::TEXT, false);
                               },
                           .close = [this](socket_type* ws, int, std::string_view) { metrics_subscribers.erase(ws); }})
          .listen(bind_addr, port, [bind_addr, port](auto* listen_socket) {
            if (listen_socket) {
              fmt::println("Remote control server listening on {}:{}", bind_addr, port);
            } else {
              fmt::println("Remote control server cannot listen on {}:{}", bind_addr, port);
            }
          });

      server.store(&ws_server, std::memory_order_relaxed);
      server_loop.store(uWS::Loop::get(), std::memory_order_relaxed);
      if (enable_metrics_subscription) {
        static_cast<remote_server_sink*>(srslog::find_sink(remote_server_sink::name()))->set_server(this);
      }
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
      server_loop.load(std::memory_order_relaxed)->defer([this]() {
        // Disconnect remote sink from the server, this will prevent metrics being processed after the server is closed.
        static_cast<remote_server_sink*>(srslog::find_sink(remote_server_sink::name()))->set_server(nullptr);
        server.load()->close();
      });
      thread.join();
      server_loop.store(nullptr, std::memory_order_relaxed);
      server.store(nullptr, std::memory_order_relaxed);
    }
  }

  /// Sends the given metrics to all registered metrics subscribers.
  void send_metrics(std::string metrics_)
  {
    server_loop.load(std::memory_order_relaxed)->defer([metrics = std::move(metrics_), this]() {
      for (auto* subscriber : metrics_subscribers) {
        subscriber->send(metrics, uWS::OpCode::TEXT, false);
      }
    });
  }

private:
  /// Adds the client that invoked this method to the metrics subscription list.
  void subscribe_metrics_client()
  {
    srsran_assert(current_cmd_client, "Invalid client");
    metrics_subscribers.emplace(current_cmd_client);
  }

  /// Removes the client that invoked this method from the metrics subscription list.
  void unsubscribe_metrics_client()
  {
    srsran_assert(current_cmd_client, "Invalid client");
    metrics_subscribers.erase(current_cmd_client);
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
};

srslog::detail::error_string remote_server_sink::write(srslog::detail::memory_buffer buffer)
{
  std::scoped_lock lock(mutex);
  if (server) {
    server->send_metrics(std::string(buffer.data(), buffer.size()));
  }
  return {};
}

} // namespace

void srsran::app_services::initialize_json_channel()
{
  /// Log channel name for the JSON type.
  static std::string json_channel_name = "JSON_channel";

  srslog::install_custom_sink(remote_server_sink::name(),
                              std::make_unique<remote_server_sink>(srslog::create_text_formatter()));
  srslog::log_channel& json_channel =
      srslog::fetch_log_channel(json_channel_name, *srslog::find_sink(remote_server_sink::name()), {});
  json_channel.set_enabled(true);
}

std::unique_ptr<remote_server>
srsran::app_services::create_remote_server(const remote_control_appconfig&       cfg,
                                           span<std::unique_ptr<remote_command>> commands)
{
  if (!cfg.enabled) {
    return nullptr;
  }

  return std::make_unique<remote_server_impl>(cfg.bind_addr, cfg.port, cfg.enable_metrics_subscription, commands);
}
