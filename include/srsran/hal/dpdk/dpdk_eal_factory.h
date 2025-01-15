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

#include "srsran/hal/dpdk/dpdk_eal.h"

namespace srsran {
namespace dpdk {

/// Returns a dpdk_eal instance on success, otherwise returns nullptr.
std::unique_ptr<dpdk_eal> create_dpdk_eal(const std::string& args, srslog::basic_logger& logger);

} // namespace dpdk
} // namespace srsran
