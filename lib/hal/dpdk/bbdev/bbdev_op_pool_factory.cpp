/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "bbdev.h"

using namespace srsran;
using namespace dpdk;

std::unique_ptr<bbdev_op_pool> srsran::dpdk::create_bbdev_op_pool(const char*           pool_name,
                                                                  ::rte_bbdev_op_type   op_type,
                                                                  uint16_t              nof_elements,
                                                                  int                   socket,
                                                                  srslog::basic_logger& logger)
{
  // Create a new bbdev operation pool.
  ::rte_mempool* pool = ::create_op_pool(pool_name, op_type, nof_elements, socket, logger);
  if (pool == nullptr) {
    return nullptr;
  }

  return std::make_unique<bbdev_op_pool>(pool);
}
