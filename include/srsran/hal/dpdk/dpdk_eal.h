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

#include "srsran/srslog/logger.h"
#include <rte_eal.h>

namespace srsran {
namespace dpdk {

/// Interfacing to DPDK's EAL.
class dpdk_eal
{
public:
  /// Constructor.
  /// \param[in] logger SRS logger.
  explicit dpdk_eal(srslog::basic_logger& logger_) : logger(logger_) {}

  /// Destructor.
  ~dpdk_eal()
  {
    // Clean up the EAL.
    ::rte_eal_cleanup();
  }

  // Returns the internal SRS logger.
  /// \return SRS logger.
  srslog::basic_logger& get_logger() { return logger; }

private:
  /// SRS logger.
  srslog::basic_logger& logger;
};

} // namespace dpdk
} // namespace srsran
