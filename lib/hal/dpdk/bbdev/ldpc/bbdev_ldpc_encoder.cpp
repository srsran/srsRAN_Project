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

#include "bbdev_ldpc_encoder.h"
#include "srsran/srsvec/bit.h"
#include <rte_bbdev.h>
#include <rte_memcpy.h>

using namespace srsran;
using namespace dpdk;

void dpdk::set_ldpc_enc_bbdev_config(::rte_bbdev_enc_op& enc_op, const srsran::hal::hw_pdsch_encoder_configuration& cfg)
{
  // Base graph.
  enc_op.ldpc_enc.basegraph = static_cast<uint8_t>(cfg.base_graph_index);
  // Modulation.
  enc_op.ldpc_enc.q_m = static_cast<uint8_t>(cfg.modulation);
  // RV index.
  enc_op.ldpc_enc.rv_index = static_cast<uint8_t>(cfg.rv);

  // Check if CB mode or TB mode is to be used.
  if (cfg.cb_mode) {
    // CB mode is currently used. CB size is in bytes.
    enc_op.ldpc_enc.code_block_mode = 1;

    // Set the CB-specific parameters.
    enc_op.ldpc_enc.cb_params.e = static_cast<uint32_t>(cfg.rm_length);
  } else {
    // TB mode is currently used. TB size is in bytes.
    enc_op.ldpc_enc.code_block_mode = 0;

    // Set the TB-specific parameters.
    enc_op.ldpc_enc.tb_params.c   = static_cast<uint8_t>(cfg.nof_segments);
    enc_op.ldpc_enc.tb_params.cab = static_cast<uint8_t>(cfg.nof_short_segments);
    enc_op.ldpc_enc.tb_params.ea  = static_cast<uint32_t>(cfg.cw_length_a);
    enc_op.ldpc_enc.tb_params.eb  = static_cast<uint32_t>(cfg.cw_length_b);
    // The offset to first CB in inbound mbuf data is assumed to be 0.
    enc_op.ldpc_enc.tb_params.r = 0;
  }

  // Lifting size.
  enc_op.ldpc_enc.z_c = static_cast<uint16_t>(cfg.lifting_size);
  unsigned K          = 0;
  if (enc_op.ldpc_enc.basegraph == 1) {
    K = 22 * enc_op.ldpc_enc.z_c;
  } else {
    K = 10 * enc_op.ldpc_enc.z_c;
  }
  enc_op.ldpc_enc.n_filler = cfg.nof_filler_bits;
  // Filler bits.
  // Note that providing a byte-aligned 'K - n_filler' value is a requirement.
  // Roundig up of 'n_filler' to the nearest value is done if needed.
  if ((K - enc_op.ldpc_enc.n_filler) % 8 != 0) {
    enc_op.ldpc_enc.n_filler = (cfg.nof_filler_bits + 8 - 1) & -8;
  }
  // Length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  enc_op.ldpc_enc.n_cb = static_cast<uint16_t>(cfg.Ncb);

  // Define the PDSCH encoder operation flags.
  uint32_t bitmask = 0;
  // Bit-level interleaver is always implemented by the accelerator (clear the bit).
  bitmask &= ~RTE_BBDEV_LDPC_INTERLEAVER_BYPASS;
  // Rate matching is always implemented by the accelerator (set the bit).
  bitmask |= RTE_BBDEV_LDPC_RATE_MATCH;
  // In CB mode, both TB anc CB CRC attachment is implemented by the segmenter (and, thus, is disabled in the
  // accelerator). In TB mode, TB CRC attachment is not supported by the accelerator, but the CB one is for segmented
  // TBs.
  if (!cfg.cb_mode && cfg.nof_segments > 1) {
    bitmask |= RTE_BBDEV_LDPC_CRC_24B_ATTACH;
  }
  enc_op.ldpc_enc.op_flags = bitmask;
}

bool dpdk::set_ldpc_enc_bbdev_data(::rte_bbdev_enc_op&   enc_op,
                                   ::rte_mempool&        in_mbuf_pool,
                                   ::rte_mempool&        out_mbuf_pool,
                                   span<const uint8_t>   data,
                                   srslog::basic_logger& logger,
                                   span<const uint8_t>   tb_crc)
{
  // Allocate an mbuf for the input data from the received mempool.
  char*       input_data;
  ::rte_mbuf* m_head_in = ::rte_pktmbuf_alloc(&in_mbuf_pool);
  if (m_head_in == nullptr) {
    logger.error("[bbdev] Not enough mbufs in the input ldpc encoder mbuf pool.");
    return false;
  }

  // Set the memory pointers for the input data.
  enc_op.ldpc_enc.input.data   = m_head_in;
  enc_op.ldpc_enc.input.offset = 0; // Note that a dedicated mbuf is used per operation.
  enc_op.ldpc_enc.input.length = 0;

  // Allocate the required bytes in the mbuf and update the memory pointers.
  uint16_t data_len   = data.size();
  uint16_t tb_crc_len = tb_crc.size();
  input_data          = ::rte_pktmbuf_append(m_head_in, data_len + tb_crc_len);
  if (input_data == nullptr) {
    // Free the input mbuf back to its original mbuf pools.
    ::rte_pktmbuf_free(m_head_in);
    logger.error("[bbdev] Couldn't append {} bytes to the ldpc encoder input mbuf.", data_len + tb_crc_len);
    return false;
  }
  // Copy the TB/CB data. Note that a single copy operation is performed per TB/CB. The copy assumes that the input data
  // size is below the requested mbuf size (by default set to the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
  ::rte_memcpy(input_data, data.begin(), data_len);
  // If required, copy the TB-CRC.
  if (tb_crc_len > 0) {
    ::rte_memcpy(input_data + data_len, &tb_crc[0], tb_crc_len);
  }
  enc_op.ldpc_enc.input.length += data_len + tb_crc_len;

  // Allocate an mbuf for the output data from the received mempool.
  ::rte_mbuf* m_head_out = ::rte_pktmbuf_alloc(&out_mbuf_pool);
  if (m_head_out == nullptr) {
    // Free the input mbuf back to its original mbuf pools.
    ::rte_pktmbuf_free(m_head_in);
    logger.error("[bbdev] Not enough mbufs in the output ldpc encoder mbuf pool.");
    return false;
  }

  // Set the memory pointers for the output data.
  enc_op.ldpc_enc.output.data   = m_head_out;
  enc_op.ldpc_enc.output.offset = 0; // Note that a dedicated mbuf is used per operation.
  enc_op.ldpc_enc.output.length = 0;

  return true;
}

bool dpdk::enqueue_ldpc_enc_operation(::rte_bbdev_enc_op&   enc_ops,
                                      uint16_t              num_enc_ops,
                                      uint16_t              bbdev_id,
                                      uint16_t              enc_queue_id,
                                      srslog::basic_logger& logger)
{
  // Enqueue new operations on the hardware-accelerator LDPC encoder.
  ::rte_bbdev_enc_op* ops             = &enc_ops;
  uint16_t            num_enq_enc_ops = ::rte_bbdev_enqueue_ldpc_enc_ops(bbdev_id, enc_queue_id, &ops, num_enc_ops);
  if (num_enq_enc_ops <= 0) {
    // Free the input and output mbuf back to its original mbuf pools.
    ::rte_pktmbuf_free(enc_ops.ldpc_enc.input.data);
    ::rte_pktmbuf_free(enc_ops.ldpc_enc.output.data);
    logger.error(
        "[bbdev] Couldn't enqueue new operations in the ldpc encoder. The operation will be dropped: acc [queue={}].",
        enc_queue_id);
    return false;
  }

  return true;
}

bool dpdk::dequeue_ldpc_enc_operation(::rte_bbdev_enc_op&   enc_ops,
                                      uint16_t              num_enc_ops,
                                      uint16_t              bbdev_id,
                                      uint16_t              enc_queue_id,
                                      srslog::basic_logger& logger)
{
  // Dequeue processed operations from the hardware-accelerator LDPC encoder.
  ::rte_bbdev_enc_op* ops             = &enc_ops;
  uint16_t            num_deq_enc_ops = ::rte_bbdev_dequeue_ldpc_enc_ops(bbdev_id, enc_queue_id, &ops, num_enc_ops);
  if (num_deq_enc_ops <= 0) {
    // logger.error("[bbdev] Couldn't dequeue new operations from the ldpc encoder.");
    return false;
  }

  return true;
}

void dpdk::read_ldpc_enc_bbdev_data(::rte_bbdev_enc_op& enc_op,
                                    span<uint8_t>       data,
                                    span<uint8_t>       packed_data,
                                    bool                cb_mode)
{
  ::rte_mbuf* m_head_out = enc_op.ldpc_enc.output.data;
  uint16_t    enc_len    = rte_pktmbuf_data_len(m_head_out);

  // Read the TB/CB data. Note that a single read operation is performed per TB/CB. The read assumes that the output
  // data size is below the requested mbuf size (by default set to the maximum supported value
  // RTE_BBDEV_LDPC_E_MAX_MBUF).
  uint16_t offset   = 0;
  uint8_t* enc_data = rte_pktmbuf_mtod_offset(m_head_out, uint8_t*, offset);

  // Recover the encoded data.
  ::rte_memcpy(packed_data.data(), enc_data, enc_len);

  // Unpack the accelerator outputs, while avoiding (byte-alignment) padding bits.
  uint32_t seg_length         = 0;
  uint32_t seg_offset         = 0;
  uint32_t seg_byte_offset    = 0;
  uint8_t  nof_segments       = 1;
  uint8_t  nof_short_segments = 0;
  // Check if CB mode is used. No data offset in this case (a dedicated mbuf is used).
  if (cb_mode) {
    seg_length = enc_op.ldpc_enc.cb_params.e;
  } else {
    seg_length         = enc_op.ldpc_enc.tb_params.ea;
    nof_short_segments = enc_op.ldpc_enc.tb_params.cab;
    nof_segments       = enc_op.ldpc_enc.tb_params.c;
  }

  for (uint seg_idx = 0, end_idx = nof_segments; seg_idx < end_idx; ++seg_idx) {
    uint32_t seg_byte_length = units::bits(seg_length).round_up_to_bytes().value();
    srsvec::bit_unpack(
        span<uint8_t>(data.subspan(seg_offset, seg_length)),
        ::bit_buffer::from_bytes(span<uint8_t>(&packed_data[seg_byte_offset], seg_byte_length)).first(seg_length));

    // In TB mode the segment offset needs to be udpated.
    if (!cb_mode) {
      seg_offset += seg_length;
      seg_byte_offset += seg_byte_length;
      if (seg_idx + 1 < nof_short_segments) {
        seg_length = enc_op.ldpc_enc.tb_params.ea;
      } else {
        seg_length = enc_op.ldpc_enc.tb_params.eb;
      }
    }
  }

  // Free the input and output mbuf back to its original mbuf pools.
  ::rte_pktmbuf_free(enc_op.ldpc_enc.input.data);
  ::rte_pktmbuf_free(enc_op.ldpc_enc.output.data);
}
