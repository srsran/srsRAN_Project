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

#pragma once

#include "apps/services/app_execution_metrics/executor_metrics_config.h"
#include "apps/services/app_execution_metrics/metrics/app_execution_metrics_builder.h"
#include "apps/services/metrics/metrics_config.h"
#include "srsran/support/executors/execute_until_success.h"
#include "srsran/support/executors/metrics/executor_metrics_backend.h"
#include "srsran/support/synchronization/sync_event.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace app_services {

class metrics_notifier;

/// \brief Executor metrics service.
///
/// Provides access to the executor metrics manager and manages its sessions.
class app_executor_metrics_service
{
  /// \brief Application executor metrics manager.
  ///
  /// This class controls the operation of the underlying executor metrics backend.
  class executor_metrics_manager
  {
    friend class app_executor_metrics_service;

    executor_metrics_manager(timer_manager&             timers_,
                             executor_metrics_notifier& notifier_,
                             std::chrono::milliseconds  report_period_) :
      timers(timers_), report_period(report_period_), notifier(notifier_)
    {
    }

    /// Returns executor metrics channel manager.
    executor_metrics_channel_registry& get_metrics_channel_registry() { return exec_metrics_backend; }

    ///\brief Starts the metrics backend.
    ///
    /// \param[in] metrics_exec_ - Executor used by the backend and the service synchronized start/stop.
    void start(task_executor& metrics_exec_)
    {
      metrics_exec = &metrics_exec_;

      if (stopped.exchange(false, std::memory_order_relaxed)) {
        sync_event wait_all;

        defer_until_success(*metrics_exec, timers, [this, token = wait_all.get_token()]() mutable {
          // Start the executor metrics backend.
          exec_metrics_backend.start(report_period, timers.create_unique_timer(*metrics_exec), notifier);
        });
      }
    }

    /// Stops the metrics backend.
    void stop()
    {
      if (not stopped.exchange(true, std::memory_order_relaxed)) {
        sync_event wait_all;
        defer_until_success(
            *metrics_exec, timers, [this, token = wait_all.get_token()]() mutable { exec_metrics_backend.stop(); });
      }
    }

    /// Application timers manager.
    timer_manager& timers;
    /// Executor metrics report period.
    std::chrono::milliseconds report_period;
    /// An executor passed to the backend. It is also used for starting/stopping the service.
    task_executor* metrics_exec;
    /// Backend processing executors metrics.
    executor_metrics_backend exec_metrics_backend;
    /// Notifier used by the backend to notify metrics.
    executor_metrics_notifier& notifier;
    /// Operation status of this service.
    std::atomic<bool> stopped{true};
  };

  /// Callback called on session destruction.
  struct session_close_callback {
    session_close_callback(app_executor_metrics_service* parent_ = nullptr) : parent(parent_) {}

    void operator()()
    {
      if (parent) {
        parent->close_session();
      }
    }

    app_executor_metrics_service* parent;
  };

public:
  /// RAII class managing the lifetime of the \c executor_metrics_manager.
  class executor_metrics_session
  {
    friend class app_executor_metrics_service;

    /// Constructor starts the metrics manager.
    executor_metrics_session(executor_metrics_manager* manager_        = nullptr,
                             task_executor*            metrics_exec    = nullptr,
                             session_close_callback    scope_exit_func = {}) :
      manager(manager_), scope_exit_function(scope_exit_func)
    {
      if (!manager) {
        return;
      }

      report_error_if_not(metrics_exec != nullptr,
                          "Task executor must be provided in order to create an execution metrics session");
      manager->start(*metrics_exec);
    }

  public:
    /// Destructor stops the metrics manager.
    ~executor_metrics_session()
    {
      if (manager) {
        manager->stop();
        scope_exit_function();
      }
    }

    /// Disable copy and move-assignment of the session.
    executor_metrics_session(const executor_metrics_session&)            = delete;
    executor_metrics_session& operator=(const executor_metrics_session&) = delete;
    executor_metrics_session& operator=(executor_metrics_session&&)      = delete;

    executor_metrics_session(executor_metrics_session&& other) noexcept :
      manager(std::exchange(other.manager, nullptr)), scope_exit_function(std::exchange(other.scope_exit_function, {}))
    {
    }

  private:
    executor_metrics_manager* manager;
    session_close_callback    scope_exit_function;
  };

  app_executor_metrics_service(timer_manager&             timers_,
                               executor_metrics_notifier& notifier_,
                               std::chrono::milliseconds  report_period_) :
    manager(timers_, notifier_, report_period_)
  {
  }

  /// Returns executor metrics channel manager.
  executor_metrics_channel_registry& get_metrics_channel_registry() { return manager.get_metrics_channel_registry(); }

  /// Creates executor metrics manager session.
  executor_metrics_session create_session(task_executor& metrics_executor)
  {
    // Verify there is no other active session using the same metrics manager.
    bool expected = false;
    if (!session_active.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
      srslog::fetch_basic_logger("METRICS").warning(
          "Failed to create executor metrics session as only one running session is allowed");
      return {};
    }

    return {&manager, &metrics_executor, {this}};
  }

  /// Creates a dummy session, which doesn't start an executor metrics manager.
  static executor_metrics_session create_dummy_session() { return {}; }

private:
  /// Callback called by the session destructor when it gets out of execution scope.
  void close_session() noexcept { session_active.store(false, std::memory_order_release); }
  /// Tracks active session.
  std::atomic<bool> session_active{false};

  executor_metrics_manager manager;
};

/// Aggregates the executor metrics service with the related application metrics configuration.
struct executor_metrics_service_and_metrics {
  std::unique_ptr<app_executor_metrics_service> service;
  std::vector<app_services::metrics_config>     metrics;
  /// Executor metrics channel registry is initialized by the service when the executor metrics are enabled.
  executor_metrics_channel_registry* channel_registry = nullptr;
};

/// Builds executor metrics service and related metrics.
inline executor_metrics_service_and_metrics
build_executor_metrics_service(app_services::metrics_notifier& metrics_notifier,
                               timer_manager&                  timers,
                               const executor_metrics_config&  app_cfg)
{
  executor_metrics_service_and_metrics service_cfg;
  if (!app_cfg.enable_executor_metrics) {
    return service_cfg;
  }

  // Build app metrics configuration.
  auto& metrics  = service_cfg.metrics.emplace_back();
  auto& notifier = build_app_execution_metrics_config(metrics, metrics_notifier, app_cfg.common_metrics_cfg);

  // Create executor metrics service.
  service_cfg.service = std::make_unique<app_executor_metrics_service>(timers, notifier, app_cfg.report_period_ms);
  service_cfg.channel_registry = &service_cfg.service->get_metrics_channel_registry();

  return service_cfg;
}

} // namespace app_services
} // namespace srsran
