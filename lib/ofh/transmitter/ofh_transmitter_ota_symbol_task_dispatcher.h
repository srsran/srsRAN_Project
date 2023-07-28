/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace ofh {

/// OTA symbol task dispatcher for the transmitter.
class transmitter_ota_symbol_task_dispatcher : public ota_symbol_handler
{
public:
  transmitter_ota_symbol_task_dispatcher(task_executor&      executor_,
                                         ota_symbol_handler& window_checker_,
                                         ota_symbol_handler& symbol_handler_) :
    executor(executor_), window_checker(window_checker_), symbol_handler(symbol_handler_)
  {
  }

  void handle_new_ota_symbol(slot_symbol_point symbol_point) override
  {
    window_checker.handle_new_ota_symbol(symbol_point);

    executor.execute([&, symbol_point]() { symbol_handler.handle_new_ota_symbol(symbol_point); });
  }

private:
  task_executor&      executor;
  ota_symbol_handler& window_checker;
  ota_symbol_handler& symbol_handler;
};

} // namespace ofh
} // namespace srsran
