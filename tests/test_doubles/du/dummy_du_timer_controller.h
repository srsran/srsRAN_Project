/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_timer_controller.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

class dummy_du_timer_controller : public du_timer_controller
{
public:
  dummy_du_timer_controller(timer_manager& timers_) : timers(timers_) {}

  std::optional<unique_ticker> create_manual_ticker() override { return std::make_optional<unique_ticker>(*this); }

  timer_manager& get_timer_manager() override { return timers; }

private:
  void do_reset_ticker() override
  {
    // do nothing
  }

  void do_tick() override { timers.tick(); }

private:
  timer_manager& timers;
};

} // namespace srs_du
} // namespace srsran
