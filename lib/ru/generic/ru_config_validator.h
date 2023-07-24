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

#include "srsran/ru/ru_generic_configuration.h"

namespace srsran {

/// \brief Helper function that validates the pointer members of the given Radio Unit configuration. Returns true on
/// success, otherwise false.
///
/// The pointers to baseband gateway, rx symbol notifier, timing notifier and error notifier are not being checked as
/// the Radio Unit factory still needs to create the adapters and set them in the lower PHY configuration.
inline bool is_valid(const ru_generic_configuration& config)
{
  if (!config.symbol_notifier) {
    return false;
  }

  if (!config.timing_notifier) {
    return false;
  }

  if (!config.rf_logger) {
    return false;
  }

  if (!config.radio_exec) {
    return false;
  }

  if (!config.statistics_printer_executor) {
    return false;
  }

  for (const auto& low_cfg : config.lower_phy_config) {
    if (!low_cfg.rx_task_executor) {
      return false;
    }

    if (!low_cfg.tx_task_executor) {
      return false;
    }

    if (!low_cfg.dl_task_executor) {
      return false;
    }

    if (!low_cfg.ul_task_executor) {
      return false;
    }

    if (!low_cfg.prach_async_executor) {
      return false;
    }
  }

  return true;
}

} // namespace srsran
