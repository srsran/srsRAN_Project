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
/// \brief Definition of the common functions used by all bbdeb-based hardware-accelerated channel processor
/// implementations.

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include <rte_bbdev.h>
#include <rte_bbdev_op.h>
#include <rte_mempool.h>

namespace srsran {
namespace dpdk {

/// bbdev initialization.
/// \param[in]  cfg    Configuration of the bbdev-based hardware-accelerator.
/// \param[in]  logger SRS logger.
/// \return BBDEV device information if successful, otherwise \c nullopt.
expected<::rte_bbdev_info> bbdev_start(const bbdev_acc_configuration& cfg, srslog::basic_logger& logger);

/// bbdev shutdown management.
/// \param[in]  dev_id ID of the bbdev-based hardware-accelerator.
/// \param[in]  logger SRS logger.
bool bbdev_stop(unsigned dev_id, srslog::basic_logger& logger);

/// Creates a new op pool for a given hardware-accelerator device.
/// \param[in]  pool_name     Unique ID for the new mbuf pool.
/// \param[in]  op_type       Type of bbdev op.
/// \param[in]  nof_elements  Number of elements in the op pool.
/// \param[in]  socket        Socket used by the bbdev accelerator.
/// \param[in]  logger        SRS logger.
/// \return Pointer to the created op pool.
rte_mempool* create_op_pool(const char*           pool_name,
                            ::rte_bbdev_op_type   op_type,
                            uint16_t              nof_elements,
                            int                   socket,
                            srslog::basic_logger& logger);

} // namespace dpdk
} // namespace srsran
