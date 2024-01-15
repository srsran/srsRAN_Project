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

/// \file
/// \brief Definition of the common functions used by all ACC100-accelerated channel processor function implementations.

#pragma once

#include "srsran/hal/dpdk/dpdk_eal.h"
#include "srsran/srslog/logger.h"
#include <rte_eal.h>
#include <rte_mbuf.h>

namespace srsran {
namespace dpdk {

/// \file
/// \brief Common DPDK support functions.

/// EAL initialization (called at application init).
/// \param[in]  argc, argv Command line arguments.
/// \param[in]  logger     SRS logger.
bool eal_init(int argc, char** argv, srslog::basic_logger& logger);

/// Creates a new memory pool for a given hardware-accelerator device.
/// \param[in]  pool_name          Unique ID for the new mbuf pool.
/// \param[in]  socket             Socket used by the bbdev accelerator.
/// \param[in]  n_mbuf             Number of elements in the mbuf pool.
/// \param[in]  mempool_cache_size Size of the per-core object cache.
/// \param[in]  mbuf_data_size     Size of data buffer in each mbuf.
/// \param[in]  logger             SRS logger.
/// \return Pointer to the created mbuf pool.
rte_mempool* create_mem_pool(const char*           pool_name,
                             int                   socket,
                             unsigned              n_mbuf,
                             unsigned              mempool_cache_size,
                             unsigned              mbuf_data_size,
                             srslog::basic_logger& logger);

} // namespace dpdk
} // namespace srsran
