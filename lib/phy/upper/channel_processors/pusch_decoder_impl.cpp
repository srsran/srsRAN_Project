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
#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;

// Number of bits in one byte.
static constexpr unsigned BITS_PER_BYTE = 8;

// Maximum TBS that implies a 16-bit CRC.
constexpr unsigned MAX_BITS_CRC16 = 3824;

// Number of bits in the long CRC. A CRC of this length is used either for TB CRCs, when the TB is longer than
// MAX_BITS_CRC16, or as a codeblock CRC, when the TB consists of multiple codeblocks.
constexpr unsigned LONG_CRC_LENGTH = 24;

// Maximum accepted transport block size.
static constexpr unsigned MAX_TBS = 1277992;

// Select the CRC for the decoder based on the TBS and the number of codeblocks.
crc_calculator* select_crc(pusch_decoder_impl::sch_crc& crcs, unsigned tbs, unsigned nof_blocks)
{
  if (nof_blocks > 1) {
    return crcs.crc24B.get();
  }
  // From here, we have only 1 codeblock.
  if (tbs > MAX_BITS_CRC16) {
    return crcs.crc24A.get();
  }
  // One short codeblock.
  return crcs.crc16.get();
}

// Computes the TB size in bits including the CRC. The CRC is accounted for only when there are multiple codeblocks.
// Otherwise, one the TB consists of a single codeblock, tb_and_crc_size = tb_size. The input is the TB size (in bits)
// and the number of codeblocks.
static unsigned get_tb_and_crc_size(unsigned tb_size, unsigned nof_cbs)
{
  unsigned tb_and_crc_size = tb_size;
  // If only one codeblock is transmitted, the CRC is taken into account by the decoder. If more than one codeblock is
  // transmitted, there is an extra CRC of length 24 bits.
  if (nof_cbs > 1) {
    tb_and_crc_size += LONG_CRC_LENGTH;
  }

  return tb_and_crc_size;
};

// Returns, in order, the codeblock length, the message length and the number of data bits.
// The message length is the number of systematic bits of the codeblock. This includes data and, if applicable, CRC,
// zero padding and filler bits.
static std::tuple<unsigned, unsigned, unsigned> get_cblk_bit_breakdown(const codeblock_metadata& cb_meta)
{
  // Get codeblock length, without rate matching.
  unsigned cb_length = cb_meta.cb_specific.full_length;

  // Compute the message length.
  // BG1 has rate 1/3 and BG2 has rate 1/5.
  constexpr unsigned INVERSE_BG1_RATE = 3;
  constexpr unsigned INVERSE_BG2_RATE = 5;
  unsigned           inverse_rate =
      (cb_meta.tb_common.base_graph == ldpc::base_graph_t::BG1) ? INVERSE_BG1_RATE : INVERSE_BG2_RATE;
  unsigned msg_length = cb_length / inverse_rate;

  // Number of data bits (no CRC, no filler bits - may contain zero-padding).
  unsigned nof_data_bits = msg_length - cb_meta.cb_specific.nof_crc_bits - cb_meta.cb_specific.nof_filler_bits;

  return {cb_length, msg_length, nof_data_bits};
}

static optional<unsigned> decode_cblk(span<uint8_t>                       output,
                                      span<const log_likelihood_ratio>    input,
                                      ldpc_decoder*                       dec,
                                      crc_calculator*                     crc,
                                      const codeblock_metadata&           cb_meta,
                                      const pusch_decoder::configuration& cfg)
{
  ldpc_decoder::configuration::algorithm_details alg_details = {};
  alg_details.max_iterations                                 = cfg.nof_ldpc_iterations;
  // As for the other alg_details, we use the default values.

  if (cfg.use_early_stop) {
    return dec->decode(output, input, crc, {cb_meta, alg_details});
  }

  // Without early stop, first decode and then check the CRC.
  dec->decode(output, input, nullptr, {cb_meta, alg_details});

  // Discard filler bits.
  unsigned nof_significant_bits = output.size() - cb_meta.cb_specific.nof_filler_bits;
  if (crc->calculate_bit(output.first(nof_significant_bits)) == 0) {
    return cfg.nof_ldpc_iterations;
  }

  return {};
}

// Checks the TB CRC (multiple codeblocks case).
static bool is_tb_crc_ok(span<const uint8_t>              transport_block_bytes,
                         span<const uint8_t>              crc_bits,
                         std::unique_ptr<crc_calculator>& crc_calc)
{
  crc_calculator_checksum_t checksum_tbs = crc_calc->calculate_byte(transport_block_bytes);
  unsigned                  checksum_cmp = srsvec::bit_pack(crc_bits, LONG_CRC_LENGTH);
  return (checksum_cmp == checksum_tbs);
}

void pusch_decoder_impl::decode(span<uint8_t>                    transport_block,
                                statistics&                      stats,
                                rx_softbuffer*                   soft_codeword,
                                span<const log_likelihood_ratio> llrs,
                                const configuration&             cfg)
{
  // Temporary buffer to store the rate-matched codeblocks (represented by LLRs) and their metadata.
  static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblock_llrs = {};
  // Recall that the TB is in packed format.
  unsigned tb_size = transport_block.size() * BITS_PER_BYTE;
  segmenter->segment(codeblock_llrs, llrs, tb_size, cfg.segmenter_cfg);

  unsigned nof_cbs = codeblock_llrs.size();
  srsran_assert(nof_cbs == soft_codeword->get_nof_codeblocks(), "Wrong number of codeblocks.");

  unsigned tb_and_crc_size = get_tb_and_crc_size(tb_size, nof_cbs);

  // Temporary buffer to store the unpacked transport block (and, if applicable, its CRC).
  static_vector<uint8_t, MAX_TBS + LONG_CRC_LENGTH> tmp_tb_bits(tb_and_crc_size);

  // Select CRC calculator for inner codeblock checks.
  crc_calculator* block_crc = select_crc(crc_set, tb_size, nof_cbs);

  span<bool> cb_crcs         = soft_codeword->get_codeblocks_crc();
  unsigned   tb_offset       = 0;
  stats.nof_codeblocks_total = nof_cbs;
  stats.ldpc_decoder_stats.reset();
  for (unsigned cb_id = 0; cb_id != nof_cbs; ++cb_id) {
    const auto& cb_llrs = codeblock_llrs[cb_id].first;
    const auto& cb_meta = codeblock_llrs[cb_id].second;
    srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

    // Get codeblock length, without rate matching, the message length and the number of data bits (no CRC, no filler
    // bits - may contain zero-padding).
    unsigned cb_length = 0, msg_length = 0, nof_data_bits = 0;
    std::tie(cb_length, msg_length, nof_data_bits) = get_cblk_bit_breakdown(cb_meta);

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
      span<log_likelihood_ratio> codeblock = soft_codeword->get_codeblock_soft_bits(cb_id, cb_length);

      // Dematch the new LLRs and combine them with the ones from previous transmissions.
      dematcher->rate_dematch(codeblock, cb_llrs, cfg.new_data, cb_meta);

      // Try to decode.
      optional<unsigned> nof_iters = decode_cblk(message, codeblock, decoder.get(), block_crc, cb_meta, cfg);

      if (nof_iters.has_value()) {
        // If successful decoding, flag the CRC, record number of iterations and copy bits to the TB buffer.
        cb_crcs[cb_id] = true;
        stats.ldpc_decoder_stats.update(nof_iters.value());
        std::copy_n(message.begin(), nof_new_bits, this_segment.begin());
      }
    } else {
      std::copy_n(message.begin(), nof_new_bits, this_segment.begin());
    }
    tb_offset += nof_new_bits;
  }
  srsran_assert(tb_offset == tb_and_crc_size, "All TB bits should be filled at this point.");

  stats.tb_crc_ok = false;
  if (nof_cbs == 1) {
    // When only one codeblock, the CRC of codeblock and transport block are the same.
    stats.tb_crc_ok = cb_crcs[0];
    if (stats.tb_crc_ok) {
      srsvec::bit_pack(transport_block, span<uint8_t>(tmp_tb_bits).first(tb_size));
    }
  } else if (std::all_of(cb_crcs.begin(), cb_crcs.end(), [](bool a) { return a; })) {
    // When more than one codeblock, we need to check the global transport block CRC. Note that there is no need to
    // compute the CRC if any of the codeblocks was not decoded correctly.
    srsvec::bit_pack(transport_block, span<uint8_t>(tmp_tb_bits).first(tb_size));

    if (is_tb_crc_ok(transport_block, span<const uint8_t>(tmp_tb_bits).last(LONG_CRC_LENGTH), crc_set.crc24A)) {
      stats.tb_crc_ok = true;
    } else {
      // If the checksum is wrong, then at least one of the codeblocks is a false negative. Reset all of them.
      soft_codeword->reset_codeblocks_crc();
    }
  }
  // In case there are multiple codeblocks and at least one has a corrupted codeblock CRC, nothing to do.
}
