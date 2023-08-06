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
