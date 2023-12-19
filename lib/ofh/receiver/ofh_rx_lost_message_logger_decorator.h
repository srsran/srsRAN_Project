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

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"

namespace srsran {

class task_executor;

namespace ofh {

/// Open Fronthaul lost received message logger decorator.
///
/// The decorator will check the slot when the reception window closes and report if OFH messages were expected for that
/// slot.
class rx_lost_message_logger_decorator : public ota_symbol_boundary_notifier
{
public:
  rx_lost_message_logger_decorator(const du_rx_window_timing_parameters&      params,
                                   cyclic_prefix                              cp,
                                   std::chrono::duration<double, std::nano>   symbol_duration,
                                   srslog::basic_logger&                      logger_,
                                   ota_symbol_boundary_notifier&              notifier_,
                                   std::shared_ptr<prach_context_repository>  prach_context_repo_ptr_,
                                   std::shared_ptr<uplink_context_repository> ul_context_repo_ptr_);

  // See interface for documentation.
  void on_new_symbol(slot_symbol_point symbol_point) override;

private:
  /// Logs the lost messages for the given symbol point.
  void log_lost_message(slot_symbol_point symbol_point);

private:
  const unsigned                             nof_symbols_rx_window_closes;
  const unsigned                             nof_symbols;
  slot_point                                 last_slot_checked;
  srslog::basic_logger&                      logger;
  ota_symbol_boundary_notifier&              notifier;
  std::shared_ptr<prach_context_repository>  prach_context_repo_ptr;
  prach_context_repository&                  prach_context_repo;
  std::shared_ptr<uplink_context_repository> ul_context_repo_ptr;
  uplink_context_repository&                 ul_context_repo;
};

} // namespace ofh
} // namespace srsran
