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

#include "srsran/adt/span.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace ofh {

/// OTA symbol task dispatcher for the transmitter.
class transmitter_ota_symbol_task_dispatcher : public ota_symbol_boundary_notifier
{
public:
  transmitter_ota_symbol_task_dispatcher(task_executor&                executor_,
                                         ota_symbol_boundary_notifier& window_checker_,
                                         ota_symbol_boundary_notifier& symbol_handler_) :
    executor(executor_), window_checker(window_checker_), symbol_handler(symbol_handler_)
  {
  }

  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override
  {
    window_checker.on_new_symbol(symbol_point_context);

    if (!executor.execute([&, symbol_point_context]() { symbol_handler.on_new_symbol(symbol_point_context); })) {
      srslog::fetch_basic_logger("OFH").warning(
          "Failed to dispatch new symbol task in the message transmitter for slot '{}' and symbol '{}'",
          symbol_point_context.symbol_point.get_slot(),
          symbol_point_context.symbol_point.get_symbol_index());
    }
  }

private:
  task_executor&                executor;
  ota_symbol_boundary_notifier& window_checker;
  ota_symbol_boundary_notifier& symbol_handler;
};

} // namespace ofh
} // namespace srsran
