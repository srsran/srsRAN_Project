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

#include "bbdev_ldpc_decoder.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include <rte_bbdev.h>
#include <rte_memcpy.h>

using namespace srsran;
using namespace dpdk;

void dpdk::set_ldpc_dec_bbdev_config(::rte_bbdev_dec_op&                                dec_op,
                                     const srsran::hal::hw_pusch_decoder_configuration& cfg,
                                     bool                                               ext_softbuffer)
{
  // Base graph.
  dec_op.ldpc_dec.basegraph = static_cast<uint8_t>(cfg.base_graph_index);
  // Modulation.
  // Note that in terms of LDPC decoding, 'PI_2_BPSK' and 'BPSK' are the same.
  unsigned modulation = static_cast<uint8_t>(cfg.modulation);
  dec_op.ldpc_dec.q_m = modulation;
  if (modulation == 0) {
    dec_op.ldpc_dec.q_m = 1;
  }
  // RV index.
  dec_op.ldpc_dec.rv_index = static_cast<uint8_t>(cfg.rv);

  // CB mode is currently forced (TB mode is not yet supported by bbdev/ACC100). CB size is in bytes.
  dec_op.ldpc_dec.code_block_mode = 1;

  // Set the CB-specific parameters.
  dec_op.ldpc_dec.cb_params.e = static_cast<uint32_t>(cfg.cw_length);

  // Lifting size.
  dec_op.ldpc_dec.z_c = static_cast<uint16_t>(cfg.lifting_size);
  // Filler bits.
  dec_op.ldpc_dec.n_filler = static_cast<uint16_t>(cfg.nof_filler_bits);
  // Length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  dec_op.ldpc_dec.n_cb = static_cast<uint16_t>(cfg.Ncb);
  // Maximum number of iterations to perform in decoding CB.
  dec_op.ldpc_dec.iter_max = static_cast<uint8_t>(cfg.max_nof_ldpc_iterations);

  // TBD: a certain operation mode is assumed, see if a detailed hardware-accelerator capabilities checking is required?
  // Define the PDSCH decoder operation flags.
  uint32_t bitmask = 0;
  // Only CRC24B checking is supported by the hardware-accelerator.
  if (cfg.cb_crc_type == srsran::hal::hw_dec_cb_crc_type::CRC24B) {
    // Codeblock CRC-24B checking is always implemented by the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_CRC_TYPE_24B_CHECK;
    // Dropping of last CRC bits decoding output is always implemented in the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_CRC_TYPE_24B_DROP;
  }
  // Bit-level de-interleaver is always implemented by the accelerator (clear the bit).
  bitmask &= ~RTE_BBDEV_LDPC_DEINTERLEAVER_BYPASS;
  // Input HARQ combining data is only needed in case of a retransmission.
  if (!cfg.new_data) {
    // HARQ combined input stream is enabled in the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_HQ_COMBINE_IN_ENABLE;

    // Check if the soft-buffer is implemented in the accelerator or in the host.
    if (ext_softbuffer) {
      // HARQ input is provided from the accelerator's internal memory (set the bit).
      bitmask |= RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_IN_ENABLE;
    }
  }
  // HARQ combined output stream is always enabled in the accelerator (set the bit).
  bitmask |= RTE_BBDEV_LDPC_HQ_COMBINE_OUT_ENABLE;
  // Check if the soft-buffer is implemented in the accelerator or in the host.
  if (ext_softbuffer) {
    // HARQ output is provided to the accelerator's internal memory (set the bit).
    bitmask |= RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_OUT_ENABLE;
  }
  // LDPC decoder is always implemented by the accelerator (clear the bit).
  bitmask &= ~RTE_BBDEV_LDPC_DECODE_BYPASS;
  // If early stop is requested, enable it in the accerlerator (set the bit, otherwise clear it).
  if (cfg.use_early_stop) {
    bitmask |= RTE_BBDEV_LDPC_ITERATION_STOP_ENABLE;
  } else {
    bitmask &= ~RTE_BBDEV_LDPC_ITERATION_STOP_ENABLE;
  }
  dec_op.ldpc_dec.op_flags = bitmask;
}

bool dpdk::set_ldpc_dec_bbdev_data(::rte_bbdev_dec_op&   dec_op,
                                   ::rte_mempool&        in_mbuf_pool,
                                   ::rte_mempool&        harq_in_mbuf_pool,
                                   ::rte_mempool&        out_mbuf_pool,
                                   ::rte_mempool&        harq_out_mbuf_pool,
                                   span<const int8_t>    data,
                                   span<const int8_t>    soft_data,
                                   bool                  new_data,
                                   bool                  ext_softbuffer,
                                   uint32_t              soft_data_len,
                                   unsigned              cb_index,
                                   unsigned              absolute_cb_id,
                                   srslog::basic_logger& logger)
{
  // Allocate an mbuf for the input data from the received mempool.
  char*       input_data;
  ::rte_mbuf* m_head_in = ::rte_pktmbuf_alloc(&in_mbuf_pool);
  if (m_head_in == nullptr) {
    logger.error("[bbdev] Not enough mbufs in the input ldpc decoder mbuf pool.");
    return false;
  }

  // Set the memory pointers for the input data, accounting for the offset of the current segment.
  dec_op.ldpc_dec.input.data   = m_head_in;
  dec_op.ldpc_dec.input.offset = 0; // Note that a dedicated mbuf is used per CB.
  dec_op.ldpc_dec.input.length = 0;

  // Allocate the required bytes in the mbuf and update the memory pointers.
  uint16_t cw_len = data.size();
  input_data      = ::rte_pktmbuf_append(m_head_in, cw_len);
  if (input_data == nullptr) {
    // Free the input mbuf back to its original mbuf pools.
    ::rte_pktmbuf_free(m_head_in);
    logger.error("[bbdev] Couldn't append {} bytes to the ldpc decoder input mbuf.", cw_len);
    return false;
  }

  // Copy the CB data. Note that a single copy operation is performed per CB. The copy assumes that the input data size
  // is below the requested mbuf size (by default set to the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
  ::rte_memcpy(input_data, data.begin(), cw_len);
  dec_op.ldpc_dec.input.length += cw_len;

  // Harq offset for the current CB (based on its unique absolute ID).
  unsigned harq_offset = srsran::hal::HARQ_INCR.value() * absolute_cb_id;

  // Input HARQ combining data is only needed in case of a retransmission.
  if (!new_data) {
    // Copy of HARQ input data is only needed if the soft-buffer is implemented in the host.
    if (!ext_softbuffer) {
      // Allocate an mbuf for the input soft-data from the received mempool.
      char*       soft_input_data;
      ::rte_mbuf* m_head_soft_in = ::rte_pktmbuf_alloc(&harq_in_mbuf_pool);
      if (m_head_soft_in == nullptr) {
        logger.error("[bbdev] Not enough mbufs in the harq input ldpc decoder mbuf pool.");
        return false;
      }

      // Set the memory pointers for the input soft-data.
      // When using the host memory the offset is provided by the input span pointers.
      uint32_t harq_output_len                   = dec_op.ldpc_dec.harq_combined_output.length;
      dec_op.ldpc_dec.harq_combined_input.data   = m_head_soft_in;
      dec_op.ldpc_dec.harq_combined_input.offset = 0;
      dec_op.ldpc_dec.harq_combined_input.length = harq_output_len;

      // Allocate the required bytes in the mbuf and update the memory pointers.
      soft_input_data = ::rte_pktmbuf_append(m_head_soft_in, harq_output_len);
      if (soft_input_data == nullptr) {
        // Free the input and input soft-data mbufs back to its original mbuf pools.
        ::rte_pktmbuf_free(dec_op.ldpc_dec.input.data);
        ::rte_pktmbuf_free(m_head_soft_in);
        logger.error("[bbdev] Couldn't append {} bytes to the ldpc decoder harq input mbuf.", harq_output_len);
        return false;
      }

      // Copy the input soft-data located before the filler bits in the soft-buffer. Note that the copy assumes that the
      // input soft-data size is below the requested mbuf size (by default set to the maximum supported value
      // RTE_BBDEV_LDPC_E_MAX_MBUF).
      ::rte_memcpy(soft_input_data, soft_data.begin(), harq_output_len);

      //  Preload the accelerator HARQ memory and adjust the offset.
      //  Note this will introduce non-negligible latency.
      ::rte_bbdev_op_data saved_hc_in  = dec_op.ldpc_dec.harq_combined_input;
      ::rte_bbdev_op_data saved_hc_out = dec_op.ldpc_dec.harq_combined_output;
      uint32_t            saved_flags  = dec_op.ldpc_dec.op_flags;
      rte_bbdev_dec_op*   op           = &dec_op;
      dec_op.ldpc_dec.op_flags =
          RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_LOOPBACK + RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_OUT_ENABLE;
      ::rte_bbdev_enqueue_ldpc_dec_ops(0, 0, &op, 1);
      int ret = 0;
      while (ret == 0) {
        ret = ::rte_bbdev_dequeue_ldpc_dec_ops(0, 0, &op, 1);
      }
      dec_op.ldpc_dec.op_flags             = saved_flags;
      dec_op.ldpc_dec.harq_combined_input  = saved_hc_in;
      dec_op.ldpc_dec.harq_combined_output = saved_hc_out;
      // When using the accelerator's HARQ memory only pointers need updating.
    } else {
      // Update the HARQ input memory offset and length.
      // When using the accelerator memory fixed CB offsets are employed.
      dec_op.ldpc_dec.harq_combined_input.offset = harq_offset;
      dec_op.ldpc_dec.harq_combined_input.length = soft_data_len;
    }
  }

  // Allocate an mbuf for the output data from the received mempool.
  ::rte_mbuf* m_head_out = ::rte_pktmbuf_alloc(&out_mbuf_pool);
  if (m_head_out == nullptr) {
    // Free the input and input soft-data mbufs back to its original mbuf pools.
    ::rte_pktmbuf_free(dec_op.ldpc_dec.input.data);
    if (!new_data && !ext_softbuffer) {
      ::rte_pktmbuf_free(dec_op.ldpc_dec.harq_combined_input.data);
    }
    logger.error("[bbdev] Not enough mbufs in the output ldpc decoder mbuf pool.");
    return false;
  }

  // Set the memory pointers for the output data, accounting for the offset of the current segment.
  dec_op.ldpc_dec.hard_output.data   = m_head_out;
  dec_op.ldpc_dec.hard_output.offset = 0; // Note that a dedicated mbuf is used per segment.
  dec_op.ldpc_dec.hard_output.length = 0;

  // Update the HARQ output memory offset and length.
  // Host memory (offset is provided by the input span pointers).
  if (!ext_softbuffer) {
    dec_op.ldpc_dec.harq_combined_output.offset = 0;
    // Accelerator memory (fixed CB offsets are used).
  } else {
    dec_op.ldpc_dec.harq_combined_output.offset = harq_offset;
  }
  dec_op.ldpc_dec.harq_combined_output.length = 0;

  // Reading of HARQ output data is only needed if the soft-buffer is implemented in the host.
  if (!ext_softbuffer) {
    // Allocate an mbuf for the output soft-data from the received mempool.
    ::rte_mempool* harq_out_pool   = &harq_out_mbuf_pool;
    ::rte_mbuf*    m_head_soft_out = ::rte_pktmbuf_alloc(harq_out_pool);
    if (m_head_soft_out == nullptr) {
      // Free the input and input soft-data mbufs back to its original mbuf pools.
      ::rte_pktmbuf_free(dec_op.ldpc_dec.input.data);
      if (!new_data && !ext_softbuffer) {
        ::rte_pktmbuf_free(dec_op.ldpc_dec.harq_combined_input.data);
      }
      logger.error("[bbdev] Not enough mbufs in the harq output ldpc decoder mbuf pool.");
      return false;
    }

    // Set the memory pointers for the output data.
    dec_op.ldpc_dec.harq_combined_output.data = m_head_soft_out;
  }

  return true;
}

bool dpdk::enqueue_ldpc_dec_operation(::rte_bbdev_dec_op&   dec_ops,
                                      uint16_t              num_dec_ops,
                                      uint16_t              bbdev_id,
                                      uint16_t              dec_queue_id,
                                      bool                  new_data,
                                      bool                  ext_softbuffer,
                                      srslog::basic_logger& logger)
{
  // Enqueue new operations on the hardware-accelerator LDPC decoder.
  ::rte_bbdev_dec_op* ops             = &dec_ops;
  uint16_t            num_enq_dec_ops = ::rte_bbdev_enqueue_ldpc_dec_ops(bbdev_id, dec_queue_id, &ops, num_dec_ops);
  if (num_enq_dec_ops <= 0) {
    // Free the input and output mbuf back to its original mbuf pools.
    ::rte_pktmbuf_free(dec_ops.ldpc_dec.input.data);
    // Input HARQ combining data is only needed in case of a retransmission.
    if (!new_data && !ext_softbuffer) {
      ::rte_pktmbuf_free(dec_ops.ldpc_dec.harq_combined_input.data);
    }
    ::rte_pktmbuf_free(dec_ops.ldpc_dec.hard_output.data);
    if (!ext_softbuffer) {
      ::rte_pktmbuf_free(dec_ops.ldpc_dec.harq_combined_output.data);
    }
    logger.error(
        "[bbdev] Couldn't enqueue new operations in the ldpc decoder. The operation will be dropped: acc [queue={}].",
        dec_queue_id);
    return false;
  }

  return true;
}

bool dpdk::dequeue_ldpc_dec_operation(::rte_bbdev_dec_op&   dec_ops,
                                      uint16_t              num_dec_ops,
                                      uint16_t              bbdev_id,
                                      uint16_t              dec_queue_id,
                                      srslog::basic_logger& logger)
{
  // Dequeue processed operations from the hardware-accelerator LDPC decoder.
  ::rte_bbdev_dec_op* ops             = &dec_ops;
  uint16_t            num_deq_dec_ops = ::rte_bbdev_dequeue_ldpc_dec_ops(bbdev_id, dec_queue_id, &ops, num_dec_ops);
  if (num_deq_dec_ops <= 0) {
    // logger.error("[bbdev] Couldn't dequeue new operations from the ldpc decoder.");
    return false;
  }

  return true;
}

uint32_t dpdk::read_ldpc_dec_bbdev_data(::rte_bbdev_dec_op& dec_op,
                                        span<uint8_t>       data,
                                        span<int8_t>        soft_data,
                                        bool                new_data,
                                        bool                ext_softbuffer)
{
  uint32_t new_soft_data_len = 0;

  ::rte_mbuf* m_head_out = dec_op.ldpc_dec.hard_output.data;
  uint16_t    dec_len    = rte_pktmbuf_data_len(m_head_out);
  // Read the CB data. Note that a single read operation is performed per CB. The read assumes that the output data size
  // is below the requested mbuf size (by default set to the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
  uint16_t offset   = 0; // Note that a dedicated mbuf is used per segment.
  uint8_t* dec_data = rte_pktmbuf_mtod_offset(m_head_out, uint8_t*, offset);

  // Recover the decoded data.
  ::rte_memcpy(data.data(), dec_data, dec_len);

  // Update the harq length for this CB.
  new_soft_data_len = dec_op.ldpc_dec.harq_combined_output.length;

  // Only if the soft-buffer is implemented in the host then the output soft-data needs to be read from the accelerator
  // HARQ memory.
  if (!ext_softbuffer) {
    // Push back the data from the accelerator HARQ memory.
    // Note this will introduce non-negligible latency.
    int                 saved_status     = dec_op.status;
    uint8_t             saved_iter_count = dec_op.ldpc_dec.iter_count;
    uint32_t            saved_flags      = dec_op.ldpc_dec.op_flags;
    ::rte_bbdev_dec_op* op               = &dec_op;
    dec_op.ldpc_dec.op_flags =
        RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_LOOPBACK + RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_IN_ENABLE;
    dec_op.ldpc_dec.harq_combined_input.length = new_soft_data_len;
    ::rte_bbdev_enqueue_ldpc_dec_ops(0, 0, &op, 1);
    int ret = 0;
    while (ret == 0) {
      ret = ::rte_bbdev_dequeue_ldpc_dec_ops(0, 0, &op, 1);
    }
    dec_op.ldpc_dec.op_flags   = saved_flags;
    dec_op.status              = saved_status;
    dec_op.ldpc_dec.iter_count = saved_iter_count;

    // Retrieve the pointer to the mbuf used for the output soft-data.
    ::rte_mbuf* m_head_soft_out = dec_op.ldpc_dec.harq_combined_output.data;
    // Note that te read assumes that the decoded soft-data size is below the requested mbuf size (by default set to the
    // maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
    uint16_t soft_offset   = 0;
    int8_t*  dec_soft_data = rte_pktmbuf_mtod_offset(m_head_soft_out, int8_t*, soft_offset);

    // Copy the output soft-data in the soft-buffer.
    ::rte_memcpy(soft_data.data(), dec_soft_data, new_soft_data_len);
  }

  // Free the input and output mbuf back to its original mbuf pools.
  ::rte_pktmbuf_free(dec_op.ldpc_dec.input.data);
  // Input HARQ combining data is only needed in case of a retransmission.
  if (!new_data && !ext_softbuffer) {
    ::rte_pktmbuf_free(dec_op.ldpc_dec.harq_combined_input.data);
  }
  ::rte_pktmbuf_free(dec_op.ldpc_dec.hard_output.data);
  if (!ext_softbuffer) {
    ::rte_pktmbuf_free(dec_op.ldpc_dec.harq_combined_output.data);
  }

  return new_soft_data_len;
}
