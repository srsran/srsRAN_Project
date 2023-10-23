/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../support/uplink_context_repository.h"

namespace srsran {
namespace ofh {

class uplane_rx_symbol_notifier;

/// Open Fronthaul User-Plane uplink received symbol data flow notifier.
class uplane_rx_symbol_data_flow_notifier
{
public:
  uplane_rx_symbol_data_flow_notifier(srslog::basic_logger&                      logger_,
                                      std::shared_ptr<uplink_context_repository> ul_context_repo_,
                                      uplane_rx_symbol_notifier&                 notifier_) :
    logger(logger_), ul_context_repo_ptr(ul_context_repo_), ul_context_repo(*ul_context_repo_ptr), notifier(notifier_)
  {
    srsran_assert(ul_context_repo_ptr, "Invalid uplink context repository");
  }

  /// Notifies the received symbol for the given slot and symbol when all the PRBs for all the ports of the grid
  /// have been written.
  void notify_received_symbol(slot_point slot, unsigned symbol);

private:
  srslog::basic_logger&                      logger;
  std::shared_ptr<uplink_context_repository> ul_context_repo_ptr;
  uplink_context_repository&                 ul_context_repo;
  uplane_rx_symbol_notifier&                 notifier;
};

} // namespace ofh
} // namespace srsran
