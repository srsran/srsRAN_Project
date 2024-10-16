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

using namespace srsran;
using namespace dpdk;

SRSRAN_WEAK_SYMB std::shared_ptr<bbdev_acc> srsran::dpdk::create_bbdev_acc(const bbdev_acc_configuration& cfg,
                                                                           srslog::basic_logger&          logger)
{
  logger.error("[bbdev] bbdev accelerator creation failed. Cause: hardware-acceleration is not supported.");

  return nullptr;
}
