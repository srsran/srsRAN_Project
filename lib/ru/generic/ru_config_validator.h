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

#include "srsran/ru/generic/ru_generic_configuration.h"

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
