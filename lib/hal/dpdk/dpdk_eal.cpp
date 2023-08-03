/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/dpdk_eal.h"
#include "dpdk.h"

using namespace srsran;
using namespace dpdk;

dpdk_eal::dpdk_eal(srslog::basic_logger& logger_) : logger(logger_) {}

dpdk_eal::~dpdk_eal()
{
  // Clean up the EAL.
  ::rte_eal_cleanup();
}
