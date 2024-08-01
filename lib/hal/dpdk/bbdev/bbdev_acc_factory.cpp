/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "plugin_bbdev_acc_factory.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"

using namespace srsran;
using namespace dpdk;

std::unique_ptr<bbdev_acc_factory> srsran::dpdk::create_bbdev_acc_factory(std::string impl_name)
{
  // Convert implementation name to lower case.
  for (char& c : impl_name) {
    c = std::tolower(c);
  }

  // Try creating a plugin bbdev accelerator factory.
  auto factory = create_plugin_bbdev_acc_factory(impl_name);
  if (factory) {
    return factory;
  }

  // No match.
  fmt::print("Factory for bbdev accelerator type {} not found. Make sure to select a valid type.\n", impl_name);

  return nullptr;
}
