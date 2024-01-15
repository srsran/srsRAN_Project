/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../support/prach_context_repository.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane PRACH data flow notifier.
class uplane_prach_data_flow_notifier
{
public:
  uplane_prach_data_flow_notifier(srslog::basic_logger&                      logger_,
                                  std::shared_ptr<prach_context_repository>  prach_context_repo_ptr_,
                                  std::shared_ptr<uplane_rx_symbol_notifier> notifier_) :
    logger(logger_),
    prach_context_repo_ptr(prach_context_repo_ptr_),
    prach_context_repo(*prach_context_repo_ptr),
    notifier_ptr(notifier_),
    notifier(*notifier_ptr)
  {
    srsran_assert(prach_context_repo_ptr, "Invalid PRACH context repository");
  }

  /// Notifies the prach for the given slot.
  void notify_prach(slot_point slot);

private:
  srslog::basic_logger&                      logger;
  std::shared_ptr<prach_context_repository>  prach_context_repo_ptr;
  prach_context_repository&                  prach_context_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier_ptr;
  uplane_rx_symbol_notifier&                 notifier;
};

} // namespace ofh
} // namespace srsran
