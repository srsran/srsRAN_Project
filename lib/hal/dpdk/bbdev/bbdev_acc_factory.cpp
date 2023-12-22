/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "bbdev.h"

using namespace srsran;
using namespace dpdk;

std::shared_ptr<bbdev_acc> srsran::dpdk::create_bbdev_acc(const bbdev_acc_configuration& cfg,
                                                          srslog::basic_logger&          logger)
{
  // bbdev device start procedure.
  expected<::rte_bbdev_info> info = bbdev_start(cfg, logger);
  if (info.is_error()) {
    return nullptr;
  }

  return std::make_shared<bbdev_acc>(cfg, info.value(), logger);
}