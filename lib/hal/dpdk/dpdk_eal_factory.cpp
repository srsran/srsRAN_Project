/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "dpdk.h"

using namespace srsran;
using namespace dpdk;

std::unique_ptr<dpdk_eal> srsran::dpdk::create_dpdk_eal(int argc, char** argv, srslog::basic_logger& logger)
{
  // EAL initialization.
  if (!::eal_init(argc, argv, logger)) {
    return nullptr;
  }

  return std::make_unique<dpdk_eal>(logger);
}
