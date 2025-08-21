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

#include <memory>
#include <optional>

namespace srsran {

class task_executor;
class io_broker;
class timer_manager;

namespace srs_du {

/// \brief Class that manages the ticking of the DU timers. It allows switching between automatic and manual ticking
/// modes at run time.
class du_timer_controller
{
  struct ticker_deleter {
    void operator()(du_timer_controller* ts) const
    {
      if (ts != nullptr) {
        ts->do_reset_ticker();
      }
    }
  };

public:
  /// Unique handle that allows to manually tick the timers.
  class unique_ticker
  {
  public:
    explicit unique_ticker(du_timer_controller& parent_) : parent(&parent_) {}

    void tick() { parent->do_tick(); }

  private:
    std::unique_ptr<du_timer_controller, ticker_deleter> parent;
  };

  virtual ~du_timer_controller() = default;

  /// Creates a unique time source that allows to tick the timers manually.
  /// \return Manual ticker if manual mode is not already set, otherwise an empty optional.
  virtual std::optional<unique_ticker> create_manual_ticker() = 0;

  /// Retrieves repository of timers.
  virtual timer_manager& get_timer_manager() = 0;

private:
  /// Called by manual ticker on destruction.
  virtual void do_reset_ticker() = 0;

  /// Called by manual ticker to advance timers.
  virtual void do_tick() = 0;
};

/// Creates a DU timer source controller that allows switching between automatic and manual ticking modes.
std::unique_ptr<du_timer_controller>
create_du_timer_controller(timer_manager& timers, io_broker& broker, task_executor& tick_exec);

} // namespace srs_du
} // namespace srsran
