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

#include "../support/uplink_context_repository.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane uplink received symbol data flow notifier.
class uplane_rx_symbol_data_flow_notifier
{
public:
  uplane_rx_symbol_data_flow_notifier(srslog::basic_logger&                      logger_,
                                      std::shared_ptr<uplink_context_repository> ul_context_repo_,
                                      std::shared_ptr<uplane_rx_symbol_notifier> notifier_) :
    logger(logger_), ul_context_repo(std::move(ul_context_repo_)), notifier(std::move(notifier_))
  {
    srsran_assert(ul_context_repo, "Invalid uplink context repository");
    srsran_assert(notifier, "Invalid notifier pointer");
  }

  /// Notifies the received symbol for the given slot and symbol when all the PRBs for all the ports of the grid
  /// have been written.
  void notify_received_symbol(slot_point slot, unsigned symbol);

private:
  srslog::basic_logger&                      logger;
  std::shared_ptr<uplink_context_repository> ul_context_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
};

} // namespace ofh
} // namespace srsran
