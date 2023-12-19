/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_rx_lost_message_logger_decorator.h"

using namespace srsran;
using namespace ofh;

rx_lost_message_logger_decorator::rx_lost_message_logger_decorator(
    const du_rx_window_timing_parameters&      params,
    cyclic_prefix                              cp,
    std::chrono::duration<double, std::nano>   symbol_duration,
    srslog::basic_logger&                      logger_,
    ota_symbol_boundary_notifier&              notifier_,
    std::shared_ptr<prach_context_repository>  prach_context_repo_ptr_,
    std::shared_ptr<uplink_context_repository> ul_context_repo_ptr_) :
  // Add one extra symbol to the variable that controls when the reception window closes, in case that a message arrives
  // in the last symbol.
  nof_symbols_rx_window_closes(std::floor(params.Ta4_max / symbol_duration) + 1U),
  nof_symbols(get_nsymb_per_slot(cp)),
  logger(logger_),
  notifier(notifier_),
  prach_context_repo_ptr(prach_context_repo_ptr_),
  prach_context_repo(*prach_context_repo_ptr),
  ul_context_repo_ptr(ul_context_repo_ptr_),
  ul_context_repo(*ul_context_repo_ptr)
{
  srsran_assert(prach_context_repo_ptr, "Invalid PRACH context repository");
  srsran_assert(ul_context_repo_ptr, "Invalid uplink context repository");
}

void rx_lost_message_logger_decorator::on_new_symbol(slot_symbol_point symbol_point)
{
  // First notify the symbol point.
  notifier.on_new_symbol(symbol_point);

  log_lost_message(symbol_point);
}

void rx_lost_message_logger_decorator::log_lost_message(slot_symbol_point symbol_point)
{
  slot_point rx_window_closing_slot = (symbol_point - nof_symbols_rx_window_closes).get_slot();

  // Initialize the last slot checked.
  if (!last_slot_checked.valid()) {
    last_slot_checked = rx_window_closing_slot;

    return;
  }

  // Do nothing if the slot is the same.
  if (last_slot_checked == rx_window_closing_slot) {
    return;
  }

  int interval = rx_window_closing_slot - last_slot_checked;
  srsran_assert(interval > 0,
                "Last slot checked '{}' cannot be bigger than the current slot to check '{}'",
                last_slot_checked,
                rx_window_closing_slot);

  // Check for every missed slot in the repository for no notified PRACH/PUSCH.
  for (unsigned i = 0, e = interval; i != e; ++i) {
    ++last_slot_checked;

    const prach_context& prach = prach_context_repo.get(last_slot_checked);
    if (!prach.empty()) {
      logger.warning("Missed incoming User-Plane PRACH messages for slot '{}'", last_slot_checked);
    }

    for (unsigned symb_id = 0; symb_id != nof_symbols; ++symb_id) {
      const uplink_context& uplink = ul_context_repo.get(last_slot_checked, symb_id);
      if (uplink.empty()) {
        continue;
      }

      logger.warning("Missed incoming User-Plane uplink messages for slot '{}'", last_slot_checked);

      break;
    }
  }
}
