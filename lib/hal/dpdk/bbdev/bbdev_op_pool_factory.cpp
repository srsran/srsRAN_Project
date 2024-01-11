/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
