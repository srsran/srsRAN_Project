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
/// \brief Definition of the specific functions used by the bbdeb-based hardware-accelerated LDPC encoder
/// implementations.

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include <rte_bbdev_op.h>

namespace srsran {
namespace dpdk {

/// Sets the contents of the bbdev configuration structure for the hardware-accelerated LPDC encoder operations.
/// \param[out] op  bbdev encoder operation configuration structure.
/// \param[in]  cfg PDSCH encoder configuration parameter-structure.
void set_ldpc_enc_bbdev_config(::rte_bbdev_enc_op& op, const srsran::hal::hw_pdsch_encoder_configuration& cfg);

/// Sets the contents of the bbdev input mbuffer structures for the hardware-accelerated LPDC encoder operations.
/// \param[out] op            bbdev encoder operation configuration structure.
/// \param[in]  in_mbuf_pool  Input data mbuf pool for the hardware-accelerated LDPC encoder operations.
/// \param[in]  out_mbuf_pool Output data mbuf pool for the hardware-accelerated LDPC encoder operations.
/// \param[in]  data          The input data to the hardware-accelerated LDPC encoder operations.
/// \param[in]  logger        SRS logger.
/// \param[in]  tb_crc        Optional. The TB CRC computed from 'data'. Only required in TB mode.
bool set_ldpc_enc_bbdev_data(::rte_bbdev_enc_op&   op,
                             ::rte_mempool&        in_mbuf_pool,
                             ::rte_mempool&        out_mbuf_pool,
                             span<const uint8_t>   data,
                             srslog::basic_logger& logger,
                             span<const uint8_t>   tb_crc = {});

/// Enqueues new operations in the hardware-accelerated LDPC encoder.
/// \param[in] enc_ops      Array of encoder operation configuration structures.
/// \param[in] num_enc_ops  Number of encoder operations (to be enqueued - from the array).
/// \param[in] id           bbdev ID of the hardware-accelerator.
/// \param[in] enc_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in] logger       SRS logger.
bool enqueue_ldpc_enc_operation(::rte_bbdev_enc_op&   enc_ops,
                                uint16_t              ldpc_enc_ops,
                                uint16_t              id,
                                uint16_t              enc_queue_id,
                                srslog::basic_logger& logger);

/// Dequeues processed operations from the hardware-accelerated LDPC encoder.
/// \param[in] enc_ops      Array of encoder operation configuration structures.
/// \param[in] num_enc_ops  Number of encoder operations (to be dequeued - to the array).
/// \param[in] id           bbdev ID of the hardware-accelerator.
/// \param[in] enc_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in] logger       SRS logger.
bool dequeue_ldpc_enc_operation(::rte_bbdev_enc_op&   enc_ops,
                                uint16_t              ldpc_enc_ops,
                                uint16_t              id,
                                uint16_t              enc_queue_id,
                                srslog::basic_logger& logger);

/// Reads the contents of the bbdev output mbuffer structures from the hardware-accelerated LPDC encoder operations.
/// \param[in]  op          bbdev encoder operation configuration structure.
/// \param[out] data        The output data from the hardware-accelerated LDPC encoder operations.
/// \param[out] packed_data Temporary storage for the packed outputs from the hardware-accelerator.
/// \param[in]  cb_mode     Optional. Indicates if the encoder operates in CB mode (true) or TB mode (false).
void read_ldpc_enc_bbdev_data(::rte_bbdev_enc_op& op,
                              span<uint8_t>       data,
                              span<uint8_t>       packed_data,
                              bool                cb_mode = true);

} // namespace dpdk
} // namespace srsran
