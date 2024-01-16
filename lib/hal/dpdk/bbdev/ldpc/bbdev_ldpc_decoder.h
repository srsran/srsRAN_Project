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
/// \brief Definition of the specific functions used by the bbdeb-based hardware-accelerated LDPC decoder
/// implementations.

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include <rte_bbdev_op.h>

namespace srsran {
namespace dpdk {

/// Sets the contents of the bbdev configuration structure for the hardware-accelerated LPDC decoder operations.
/// \param[out] op             bbdev decoder operation configuration structure.
/// \param[in]  cfg            PUSCH decoder configuration parameter-structure.
/// \param[in]  ext_softbuffer Indicates if the soft-buffer implementation type.
void set_ldpc_dec_bbdev_config(::rte_bbdev_dec_op&                                op,
                               const srsran::hal::hw_pusch_decoder_configuration& cfg,
                               bool                                               ext_softbuffer);

/// Sets the contents of the bbdev input mbuffer structures for the hardware-accelerated LPDC decoder operations.
/// \param[out] op                 bbdev decoder operation configuration structure.
/// \param[in]  in_mbuf_pool       Input data mbuf pool for the hardware-accelerated LDPC decoder operations.
/// \param[in]  harq_in_mbuf_pool  Input soft-data mbuf pool for the hardware-accelerated LDPC decoder operations.
/// \param[in]  out_mbuf_pool      Output data mbuf pool for the hardware-accelerated LDPC decoder operations.
/// \param[in]  harq_out_mbuf_pool Output soft-data mbuf pool for the hardware-accelerated LDPC decoder operations.
/// \param[in]  data               The input data to the hardware-accelerated LDPC decoder operations.
/// \param[in]  soft_data          The input soft-data to the hardware-accelerated LDPC decoder operations.
/// \param[in]  cfg                PUSCH decoder configuration parameter-structure.
/// \param[in]  ext_softbuffer     Indicates if the soft-buffer implementation type.
/// \param[in]  soft_data_len      Length of the soft-combining input data.
/// \param[in]  cb_index           Index of the CB (within the TB).
/// \param[in]  absolute_cb_id     Unique ID of associated to the CB in the soft-buffer and HARQ ACK context entry.
/// \param[in]  logger             SRS logger.
bool set_ldpc_dec_bbdev_data(::rte_bbdev_dec_op&                                op,
                             ::rte_mempool&                                     in_mbuf_pool,
                             ::rte_mempool&                                     harq_in_mbuf_pool,
                             ::rte_mempool&                                     out_mbuf_pool,
                             ::rte_mempool&                                     harq_out_mbuf_pool,
                             span<const int8_t>                                 data,
                             span<const int8_t>                                 soft_data,
                             const srsran::hal::hw_pusch_decoder_configuration& cfg,
                             bool                                               ext_softbuffer,
                             uint32_t                                           soft_data_len,
                             unsigned                                           cb_index,
                             unsigned                                           absolute_cb_id,
                             srslog::basic_logger&                              logger);

// Enqueues new operations in the hardware-accelerated LDPC decoder.
/// \param[in] dec_ops      Array of decoder operation configuration structures.
/// \param[in] num_dec_ops  Number of decoder operations (to be enqueued - from the array).
/// \param[in] id           bbdev ID of the hardware-accelerator.
/// \param[in] dec_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in] logger       SRS logger.
bool enqueue_ldpc_dec_operation(::rte_bbdev_dec_op&   dec_ops,
                                uint16_t              ldpc_dec_ops,
                                uint16_t              id,
                                uint16_t              dec_queue_id,
                                srslog::basic_logger& logger);

/// Dequeues processed operations from the hardware-accelerated LDPC decoder.
/// \param[in] dec_ops      Array of decoder operation configuration structures.
/// \param[in] num_dec_ops  Number of decoder operations (to be dequeued - to the array).
/// \param[in] id           bbdev ID of the hardware-accelerator.
/// \param[in] dec_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in] logger       SRS logger.
bool dequeue_ldpc_dec_operation(::rte_bbdev_dec_op&   dec_ops,
                                uint16_t              ldpc_dec_ops,
                                uint16_t              id,
                                uint16_t              dec_queue_id,
                                srslog::basic_logger& logger);

/// Reads the contents of the bbdev output mbuffer structures from the hardware-accelerated LPDC decoder operations.
/// \param[in]  op             bbdev decoder operation configuration structure.
/// \param[out] data           The output data from the hardware-accelerated LDPC decoder operations.
/// \param[out] soft_data      The output soft-data from the hardware-accelerated LDPC decoder operations.
/// \param[in]  cfg            PUSCH decoder configuration parameter-structure.
/// \param[in]  ext_softbuffer Indicates if the soft-buffer implementation type.
/// \return Length of the soft-combining output data.
uint32_t read_ldpc_dec_bbdev_data(::rte_bbdev_dec_op&                                op,
                                  span<uint8_t>                                      data,
                                  span<int8_t>                                       soft_data,
                                  const srsran::hal::hw_pusch_decoder_configuration& cfg,
                                  bool                                               ext_softbuffer);

} // namespace dpdk
} // namespace srsran
