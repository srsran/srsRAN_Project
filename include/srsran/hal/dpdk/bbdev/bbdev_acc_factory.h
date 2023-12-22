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

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"

namespace srsran {
namespace dpdk {

/// Returns a bbdev_acc instance on success, otherwise returns nullptr.
std::shared_ptr<bbdev_acc> create_bbdev_acc(const bbdev_acc_configuration& cfg, srslog::basic_logger& logger);

} // namespace dpdk
} // namespace srsran