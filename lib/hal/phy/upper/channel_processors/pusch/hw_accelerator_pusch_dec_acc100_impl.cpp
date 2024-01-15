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

#include "hw_accelerator_pusch_dec_acc100_impl.h"
#include "../../../../dpdk/bbdev/ldpc/bbdev_ldpc_decoder.h"
#include "../../../../dpdk/dpdk.h"
#include "srsran/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "srsran/hal/dpdk/mbuf_pool_factory.h"

using namespace srsran;
using namespace hal;

void hw_accelerator_pusch_dec_acc100_impl::allocate_resources()
{
  int socket_id = bbdev_accelerator->get_socket_id();

  // Create bbdev op pools for the accelerated LDPC decoder operations.
  // Note that a single hardware-queue per lcore is assumed.
  unsigned nof_ldpc_dec_cores = bbdev_accelerator->get_nof_ldpc_dec_cores();
  // op pools require unique names.
  char op_pool_name[50];
  snprintf(op_pool_name, sizeof op_pool_name, "dec_op_pool_%d_%d", device_id, queue_id);
  op_pool = ::dpdk::create_bbdev_op_pool(
      op_pool_name, RTE_BBDEV_OP_LDPC_DEC, nof_ldpc_dec_cores, socket_id, bbdev_accelerator->get_logger());

  // Create new mbuf pools for both input and output data for the hardware-accelerated LDPC decoder.
  // Note that the buffers are sized taking into account that only CB mode is supported by the decoder.
  ::dpdk::mempool_config rm_mpool_cfg;
  unsigned               nof_mbuf = bbdev_accelerator->get_nof_mbuf();
  rm_mpool_cfg.mbuf_data_size     = bbdev_accelerator->get_rm_mbuf_size().value();
  rm_mpool_cfg.n_mbuf             = nof_mbuf;
  ::dpdk::mempool_config msg_mpool_cfg;
  msg_mpool_cfg.mbuf_data_size = bbdev_accelerator->get_msg_mbuf_size().value();
  msg_mpool_cfg.n_mbuf         = nof_mbuf;

  // mbuf pools require unique names.
  std::string mbuf_pool_name = fmt::format("dec_in_mbuf_pool_{}_{}", device_id, queue_id);
  in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  mbuf_pool_name = fmt::format("harq_in_mbuf_pool_{}_{}", device_id, queue_id);
  harq_in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  mbuf_pool_name = fmt::format("dec_out_mbuf_pool_{}_{}", device_id, queue_id);
  out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, msg_mpool_cfg, bbdev_accelerator->get_logger());

  mbuf_pool_name = fmt::format("harq_out_mbuf_pool_{}_{}", device_id, queue_id);
  harq_out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());
}

void hw_accelerator_pusch_dec_acc100_impl::hw_config(const hw_pusch_decoder_configuration& config, unsigned cb_index)
{
  // Save configuration.
  dec_config = config;

  // Reset the operation dropping control (first CB of the TB only).
  if (cb_index == 0) {
    drop_op.resize(config.nof_segments);
    drop_op.reset();
    harq_context_entries.resize(config.nof_segments);
  }

  // Get the HARQ buffer context entry of the CB or create a new one.
  harq_context_entries[cb_index] = harq_buffer_context->get(config.absolute_cb_id, config.new_data);

  // Store configuration in bbdev structure.
  ::dpdk::set_ldpc_dec_bbdev_config(op[cb_index], config, ext_softbuffer);
}

bool hw_accelerator_pusch_dec_acc100_impl::hw_enqueue(span<const int8_t> data,
                                                      span<const int8_t> soft_data,
                                                      unsigned           cb_index)
{
  bool     enqueued         = false;
  unsigned soft_data_len    = harq_context_entries[cb_index]->soft_data_len;
  bool     soft_data_len_ok = dec_config.new_data || (!dec_config.new_data && soft_data_len > 0);

  // Verify that the queue is not already full before trying to enqueue.
  if (nof_enqueued_op < ::dpdk::MAX_NOF_OP_IN_QUEUE && soft_data_len_ok) {
    // Set the memory-pointers for the accelerator and provide the input data and related soft-combining inputs.
    ::rte_mempool* in_pool       = in_mbuf_pool->get_pool();
    ::rte_mempool* harq_in_pool  = harq_in_mbuf_pool->get_pool();
    ::rte_mempool* out_pool      = out_mbuf_pool->get_pool();
    ::rte_mempool* harq_out_pool = harq_out_mbuf_pool->get_pool();
    ::dpdk::set_ldpc_dec_bbdev_data(op[cb_index],
                                    *in_pool,
                                    *harq_in_pool,
                                    *out_pool,
                                    *harq_out_pool,
                                    data,
                                    soft_data,
                                    dec_config,
                                    ext_softbuffer,
                                    soft_data_len,
                                    cb_index,
                                    dec_config.absolute_cb_id,
                                    bbdev_accelerator->get_logger());

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[bbdev] decoder enqueueing: cb={}, harq [new_data={}, sof_len={}, id={}], "
                "LDPC [bg={}, mod={}, seg={}, rv={}, cw={}, lifting={}, Ncb={}, filler={}, crc_len={}], acc [queue={}]",
                cb_index,
                dec_config.new_data,
                soft_data_len,
                dec_config.absolute_cb_id,
                (unsigned)dec_config.base_graph_index,
                (unsigned)dec_config.modulation,
                dec_config.nof_segments,
                dec_config.rv,
                dec_config.cw_length,
                dec_config.lifting_size,
                dec_config.Ncb,
                dec_config.nof_filler_bits,
                dec_config.cb_crc_len,
                queue_id);

    // Enqueue the LDPC decoding oepration.
    enqueued = ::dpdk::enqueue_ldpc_dec_operation(op[cb_index],
                                                  1,
                                                  device_id,
                                                  queue_id,
                                                  bbdev_accelerator->get_logger()); // TBD: single operation enqueued.

    // Update the enqueued task counter.
    if (enqueued) {
      ++nof_enqueued_op;
      // Drop the operation due to an enqueueing error.
    } else {
      drop_op.set(cb_index);
      enqueued = true;
    }
  } else if (!soft_data_len_ok) {
    drop_op.set(cb_index);
    enqueued = true;
  }

  return enqueued;
}

bool hw_accelerator_pusch_dec_acc100_impl::hw_dequeue(span<uint8_t> data,
                                                      span<int8_t>  soft_data,
                                                      unsigned      segment_index)
{
  bool dropped  = drop_op.test(segment_index);
  bool dequeued = false;

  // Verify that the queue is not already emtpy and that the operation has not been dropped before trying to dequeue.
  if (nof_enqueued_op > 0 && !dropped) {
    // Dequeue processed operations from the hardware-accelerated LDPC decoder.
    dequeued = ::dpdk::dequeue_ldpc_dec_operation(op[segment_index],
                                                  1,
                                                  device_id,
                                                  queue_id,
                                                  bbdev_accelerator->get_logger()); // TBD: single operation dequeued.

    // Check if there are new results available from the hardware accelerator.
    if (dequeued) {
      // Read the accelerator output data and related soft-combining outputs, while updating the HARQ context.
      harq_context_entries[segment_index]->soft_data_len =
          ::dpdk::read_ldpc_dec_bbdev_data(op[segment_index], data, soft_data, dec_config, ext_softbuffer);

      // Update the enqueued task counter.
      --nof_enqueued_op;
    }
  } else if (dropped) {
    dequeued = true;
  }

  return dequeued;
}

void hw_accelerator_pusch_dec_acc100_impl::hw_read_outputs(hw_pusch_decoder_outputs& out,
                                                           unsigned                  cb_index,
                                                           unsigned                  absolute_cb_id)
{
  // Save outputs.
  dec_out = out;

  srslog::basic_logger& logger = bbdev_accelerator->get_logger();

  // Operation dropped due to encoding errors.
  if (drop_op.test(cb_index)) {
    out.CRC_pass            = false;
    out.nof_ldpc_iterations = dec_config.max_nof_ldpc_iterations;
    drop_op.reset(cb_index);

    // HAL logging.
    logger.info("[bbdev] decoder skipped: cb={}, harq [new_data={}, sof_len={}, id={}], acc [queue={}]",
                cb_index,
                dec_config.new_data,
                harq_context_entries[cb_index]->soft_data_len,
                absolute_cb_id,
                queue_id);

    return;
  }

  // Check the CRC and number of utilized ldpc decoding iterations.
  int ldpc_dec_status     = op[cb_index].status;
  out.CRC_pass            = !(ldpc_dec_status & (1 << RTE_BBDEV_CRC_ERROR));
  out.nof_ldpc_iterations = op[cb_index].ldpc_dec.iter_count;

  // HAL logging.
  logger.info("[bbdev] decoder dequeued: cb={}, harq [new_data={}, sof_len={}, id={}], LDPC "
              "[crc_pass={}, "
              "ldpc_iter={}], acc [queue={}]",
              cb_index,
              dec_config.new_data,
              harq_context_entries[cb_index]->soft_data_len,
              absolute_cb_id,
              (unsigned)dec_config.cb_crc_type == 1 ? (int)out.CRC_pass : -1,
              out.nof_ldpc_iterations,
              queue_id);
}

void hw_accelerator_pusch_dec_acc100_impl::hw_free_harq_context(unsigned absolute_cb_id)
{
  harq_buffer_context->free(absolute_cb_id);
}

bool hw_accelerator_pusch_dec_acc100_impl::is_hw_external_harq_supported() const
{
  return ext_softbuffer;
}
