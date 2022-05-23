/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_decoder_impl.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;

// Number of bits in one byte.
static constexpr unsigned BITS_PER_BYTE = 8;

// Maximum accepted transport block size.
static constexpr unsigned MAX_TBS = 1277992;

std::unique_ptr<pusch_decoder> srsgnb::create_pusch_decoder()
{
  std::unique_ptr<ldpc_segmenter>      seg  = create_ldpc_segmenter();
  std::unique_ptr<ldpc_rate_dematcher> rdem = create_ldpc_rate_dematcher();
  std::unique_ptr<ldpc_decoder>        dec  = create_ldpc_decoder("generic");
  pusch_decoder_impl::sch_crc          crcs = {create_crc_calculator(crc_generator_poly::CRC16),
                                               create_crc_calculator(crc_generator_poly::CRC24A),
                                               create_crc_calculator(crc_generator_poly::CRC24B)};
  return std::make_unique<pusch_decoder_impl>(seg, rdem, dec, crcs);
}

// Select the CRC for the decoder based on the TBS and the number of codeblocks.
crc_calculator* select_crc(pusch_decoder_impl::sch_crc& crcs, unsigned tbs, unsigned nof_blocks)
{
  if (nof_blocks > 1) {
    return crcs.crc24B.get();
  }
  // From here, we have only 1 codeblock.
  constexpr unsigned MAX_BITS_CRC16 = 3824;
  if (tbs > MAX_BITS_CRC16) {
    return crcs.crc24A.get();
  }
  // One short codeblock.
  return crcs.crc16.get();
}

void pusch_decoder_impl::decode(span<uint8_t>            transport_block,
                                statistics&              info,
                                rx_softbuffer*           soft_codeword,
                                span<const int8_t>       llrs,
                                const bool               new_data,
                                const segment_config&    cfg,
                                const algorithm_details& alg_cfg)
{
  // Temporary buffer to store the rate-matched codeblocks (represented by LLRs) and their metadata.
  static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblock_llrs = {};
  // Recall that the TB is in packed format.
  unsigned tb_size = transport_block.size() * BITS_PER_BYTE;
  segmenter->segment_rx(codeblock_llrs, llrs, tb_size, cfg);

  unsigned nof_cbs = codeblock_llrs.size();
  srsran_assert(nof_cbs == soft_codeword->get_nof_codeblocks(), "Wrong number of codeblocks.");

  // BG1 has rate 1/3 and BG2 has rate 1/5.
  constexpr unsigned INVERSE_BG1_RATE = 3;
  constexpr unsigned INVERSE_BG2_RATE = 5;
  unsigned           inverse_rate =
      (codeblock_llrs[0].second.tb_common.base_graph == ldpc::base_graph_t::BG1) ? INVERSE_BG1_RATE : INVERSE_BG2_RATE;

  unsigned tb_and_crc_size = tb_size;
  // If only one codeblock is transmitted, the CRC is removed by the decoder. If more than one codeblock is transmitted,
  // there is an extra CRC of length 24 bits.
  constexpr unsigned BLOCK_CRC_LENGTH = 24;
  if (nof_cbs > 1) {
    tb_and_crc_size += BLOCK_CRC_LENGTH;
  }

  // Temporary buffer to store the unpacked transport block (and, if applicable, its CRC).
  static_vector<uint8_t, MAX_TBS + BLOCK_CRC_LENGTH> tmp_tb_bits(tb_and_crc_size);

  // Select CRC calculator for inner codeblock checks.
  crc_calculator* block_crc = select_crc(crc_set, tb_size, nof_cbs);

  span<bool> cb_crcs       = soft_codeword->get_codeblocks_crc();
  unsigned   tb_offset     = 0;
  info.nof_ldpc_iterations = 0;
  for (unsigned cb_id = 0; cb_id != nof_cbs; ++cb_id) {
    const auto& cb_llrs = codeblock_llrs[cb_id].first;
    const auto& cb_meta = codeblock_llrs[cb_id].second;
    srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

    // Get codeblock length, without rate matching.
    unsigned cb_length = cb_meta.cb_specific.full_length;
    // Compute the message length.
    unsigned msg_length = cb_length / inverse_rate;
    // Number of data bits (no CRC, no filler bits - may contain zero-padding).
    unsigned nof_data_bits = msg_length - cb_meta.cb_specific.nof_crc_bits - cb_meta.cb_specific.nof_filler_bits;

    // Get data bits from previous transmissions, if any.
    // Messages are written on a dedicated buffer associated to the softbuffer. By doing this, we keep the decoded
    // message in memory and we don't need to compute it again if there is a retransmission.
    span<uint8_t> message = soft_codeword->get_codeblock_data_bits(cb_id, msg_length);

    // Number of TB bits still "empty".
    unsigned free_tb_bits = tb_and_crc_size - tb_offset;
    // Avoid including zero-padding in the TB.
    unsigned nof_new_bits = std::min(free_tb_bits, nof_data_bits);
    // Pick a view of the segment of the transport block corresponding to the current codeblock.
    span<uint8_t> this_segment = span<uint8_t>(tmp_tb_bits).subspan(tb_offset, nof_new_bits);

    if (!cb_crcs[cb_id]) {
      // Get the LLRs from previous transmissions, if any, or a clean buffer.
      span<int8_t> codeblock = soft_codeword->get_codeblock_soft_bits(cb_id, cb_length);

      // Dematch the new LLRs and combine them with the ones from previous transmissions.
      dematcher->rate_dematch(codeblock, cb_llrs, new_data, cb_meta);

      // Try to decode
      if (auto nof_iters = decoder->decode(message, codeblock, block_crc, {cb_meta, alg_cfg})) {
        // If successful decoding, flag the CRC, record number of iterations and copy bits to the TB buffer.
        cb_crcs[cb_id] = true;
        info.nof_ldpc_iterations += static_cast<float>(nof_iters.value());
        std::copy_n(message.begin(), nof_new_bits, this_segment.begin());
      }
    } else {
      std::copy_n(message.begin(), nof_new_bits, this_segment.begin());
    }
    tb_offset += nof_new_bits;
  }
  srsran_assert(tb_offset == tb_and_crc_size, "All TB bits should be filled at this point.");
  info.nof_ldpc_iterations /= static_cast<float>(nof_cbs);

  if (nof_cbs == 1) {
    // When only one codeblock, the CRC of codeblock and transport block are the same.
    info.tb_crc_ok = cb_crcs[0];
  } else {
    // When more than one codeblock, we need to check the global transport block CRC. Note that there is no need to
    // compute the CRC if any of the codeblocks was not decoded correctly.
    info.tb_crc_ok = (std::all_of(cb_crcs.begin(), cb_crcs.end(), [](bool a) { return a; })) &&
                     (crc_set.crc24A->calculate_bit(tmp_tb_bits) == 0);
  }

  // Pack the transport block.
  srsvec::bit_pack(transport_block, span<uint8_t>(tmp_tb_bits).first(tb_size));
}
