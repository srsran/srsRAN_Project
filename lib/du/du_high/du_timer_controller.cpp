/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../include/srsran/du/du_high/du_timer_controller.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/timers.h"

using namespace srsran;
using namespace srs_du;

namespace {

class du_timer_source_impl final : public du_timer_controller
{
public:
  du_timer_source_impl(timer_manager& timers_, io_broker& broker, task_executor& tick_exec_) :
    timers(timers_), tick_exec(tick_exec_), io_source(timers, broker, tick_exec, std::chrono::milliseconds{1}, true)
  {
  }

  std::optional<unique_ticker> create_manual_ticker() override
  {
    std::optional<unique_ticker> ret;
    if (not manual_mode.exchange(true, std::memory_order_acq_rel)) {
      // This caller was the first to set manual mode.
      io_source.request_stop();
      ret.emplace(*this);
    }
    return ret;
  }

  timer_manager& get_timer_manager() override { return timers; }

private:
  void do_reset_ticker() override
  {
    io_source.resume();
    manual_mode.store(false, std::memory_order_release);
  }

  void do_tick() override
  {
    pending_ticks++;
    if (tick_exec.defer([this, nof_ticks = pending_ticks]() {
          for (unsigned i = 0; i != nof_ticks; ++i) {
            timers.tick();
          }
        })) {
      pending_ticks = 0;
    }
  }

  timer_manager& timers;
  task_executor& tick_exec;

  io_timer_source io_source;

  std::atomic<bool> manual_mode{false};
  unsigned          pending_ticks{0};
};

} // namespace

std::unique_ptr<du_timer_controller>
srs_du::create_du_timer_controller(timer_manager& timers, io_broker& broker, task_executor& tick_exec)
{
  return std::make_unique<du_timer_source_impl>(timers, broker, tick_exec);
}
