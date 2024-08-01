/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include <string>

namespace srsran {
namespace dpdk {

std::unique_ptr<bbdev_acc_factory> create_plugin_bbdev_acc_factory(std::string impl_name);

} // namespace dpdk
} // namespace srsran
