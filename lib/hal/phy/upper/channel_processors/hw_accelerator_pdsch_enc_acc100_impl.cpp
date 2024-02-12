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

#include "hw_accelerator_pdsch_enc_acc100_impl.h"
#include "../../../dpdk/bbdev/ldpc/bbdev_ldpc_encoder.h"
#include "../../../dpdk/dpdk.h"
#include "srsran/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "srsran/hal/dpdk/mbuf_pool_factory.h"

using namespace srsran;
using namespace hal;

void hw_accelerator_pdsch_enc_acc100_impl::allocate_resources()
{
  int socket_id = bbdev_accelerator->get_socket_id();

  // Create bbdev op pools for the accelerated LDPC encoder operations.
  // Note that a single hardware-queue per lcore is assumed.
  unsigned nof_ldpc_enc_cores = bbdev_accelerator->get_nof_ldpc_enc_cores();
  // op pools require unique names.
  std::string op_pool_name = fmt::format("enc_op_pool_{}_{}", device_id, queue_id);
  op_pool                  = ::dpdk::create_bbdev_op_pool(
      op_pool_name.c_str(), RTE_BBDEV_OP_LDPC_ENC, nof_ldpc_enc_cores, socket_id, bbdev_accelerator->get_logger());

  // Create new mbuf pools for both input and output data for the hardware-accelerated LDPC encoder.
  // Note that a predefined headroom length is added on top of the size required for the data in the mbufs. Also, the
  // set mbuf length represents the maximum supported TB size (also accounting for the TB CRC length) when using TB
  // mode.
  ::dpdk::mempool_config msg_mpool_cfg;
  unsigned               nof_mbuf = bbdev_accelerator->get_nof_mbuf();
  msg_mpool_cfg.mbuf_data_size    = bbdev_accelerator->get_msg_mbuf_size().value() + RTE_PKTMBUF_HEADROOM;
  msg_mpool_cfg.n_mbuf            = nof_mbuf;
  ::dpdk::mempool_config rm_mpool_cfg;
  rm_mpool_cfg.mbuf_data_size = bbdev_accelerator->get_rm_mbuf_size().value() + RTE_PKTMBUF_HEADROOM;
  rm_mpool_cfg.n_mbuf         = nof_mbuf;
  // mbuf pools require unique names.
  std::string mbuf_pool_name = fmt::format("enc_in_mbuf_pool_{}_{}", device_id, queue_id);
  in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, msg_mpool_cfg, bbdev_accelerator->get_logger());
  mbuf_pool_name = fmt::format("enc_out_mbuf_pool_{}_{}", device_id, queue_id);
  out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());
}

void hw_accelerator_pdsch_enc_acc100_impl::hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index)
{
  // Save configuration.
  enc_config = config;

  // Store configuration in bbdev structure.
  ::dpdk::set_ldpc_enc_bbdev_config(op[cb_index], config);

  // In case of TB mode, save the TB CRC.
  if (!config.cb_mode) {
    if (config.nof_tb_crc_bits == 16) {
      tb_crc.resize(2);
    }
    tb_crc = config.tb_crc;
  } else {
    tb_crc.resize(0);
  }

  // Reset the operation dropping control.
  if (cb_index == 0) {
    drop_op.resize(config.nof_segments);
    drop_op.reset();
  }
}

bool hw_accelerator_pdsch_enc_acc100_impl::hw_enqueue(span<const uint8_t> data, unsigned cb_index)
{
  bool enqueued = false;

  // Verify that the queue is not already full before trying to enqueue.
  if (nof_enqueued_op < srsran::dpdk::MAX_NOF_OP_IN_QUEUE) {
    // Set the memory-pointers for the accelerator and provide the input data.
    ::rte_mempool* in_pool  = in_mbuf_pool->get_pool();
    ::rte_mempool* out_pool = out_mbuf_pool->get_pool();
    ::dpdk::set_ldpc_enc_bbdev_data(op[cb_index], *in_pool, *out_pool, data, bbdev_accelerator->get_logger(), tb_crc);

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[bbdev] encoder enqueueing: cb={}, LDPC [bg={}, mod={}, tb={}, tb_crc={}, seg={}, short_seg={}, "
                "rv={}, cwa={}, cwb={}, lifting={}, Ncb={}, filler={}, rm={}], acc [cb_mode={}, queue={}]",
                cb_index,
                (unsigned)enc_config.base_graph_index,
                (unsigned)enc_config.modulation,
                enc_config.nof_tb_bits,
                enc_config.nof_tb_crc_bits,
                enc_config.nof_segments,
                enc_config.nof_short_segments,
                enc_config.rv,
                enc_config.cw_length_a,
                enc_config.cw_length_b,
                enc_config.lifting_size,
                enc_config.Ncb,
                enc_config.nof_filler_bits,
                enc_config.rm_length,
                enc_config.cb_mode,
                queue_id);

    // Enqueue the LDPC encoding operation.
    enqueued = ::dpdk::enqueue_ldpc_enc_operation(
        op[cb_index], 1, device_id, queue_id, bbdev_accelerator->get_logger()); // TBD: single operation enqueued.

    // Update the enqueued task counter.
    if (enqueued) {
      ++nof_enqueued_op;
      // Drop the operation due to an enqueueing error.
    } else {
      drop_op.set(cb_index);
      enqueued = true;
    }
  }

  return enqueued;
}

bool hw_accelerator_pdsch_enc_acc100_impl::hw_dequeue(span<uint8_t> data,
                                                      span<uint8_t> packed_data,
                                                      unsigned      segment_index)
{
  bool dropped  = drop_op.test(segment_index);
  bool dequeued = false;

  // Verify that the queue is not already emtpy and that the operation has not been dropped before trying to dequeue.
  if (nof_enqueued_op > 0 && !dropped) {
    // Dequeue processed operations from the hardware-accelerated LDPC encoder.
    dequeued = ::dpdk::dequeue_ldpc_enc_operation(
        op[segment_index], 1, device_id, queue_id, bbdev_accelerator->get_logger()); // TBD: single operation dequeued.

    // Read the returned results (if any).
    if (dequeued) {
      // Read the accelerator output data.
      ::dpdk::read_ldpc_enc_bbdev_data(op[segment_index], data, packed_data, enc_config.cb_mode);

      // Update the enqueued task counter.
      --nof_enqueued_op;

      // HAL logging.
      srslog::basic_logger& logger = bbdev_accelerator->get_logger();
      logger.info(
          "[bbdev] encoder dequeued: cb={}, acc [cb_mode={}, queue={}]", segment_index, enc_config.cb_mode, queue_id);
    }
  } else if (dropped) {
    drop_op.reset(segment_index);
    dequeued = true;

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info(
        "[bbdev] encoder skipped: cb={}, acc [cb_mode={}, queue={}]", segment_index, enc_config.cb_mode, queue_id);
  }

  return dequeued;
}
