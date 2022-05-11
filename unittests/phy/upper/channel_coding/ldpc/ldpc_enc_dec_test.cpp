/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Unit test for LDPC encoder and decoder.
///
/// For all possible base graphs and lifting sizes, the test extracts from a file a small set of messages and
/// corresponding codeblocks. The messages are fed to the encoder, whose output is compared to the codeblocks.
/// Similarly, the codeblocks are fed to the decoder and the resulting messages are compared to the example ones.

#include "ldpc_encoder_test_data.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>
#include <string>

/// Fixed log-likelihood ratio amplitude.
constexpr int8_t LLRS_AMPL = 10;
/// Transforms hard bits into log-likelihood ratios (with fixed amplitude).
const auto compute_llrs = [](uint8_t b) {
  return ((b == srsgnb::ldpc::FILLER_BIT) ? LLRS_AMPL : LLRS_AMPL * (1 - 2 * b));
};

/// Checks whether two messages are equal: filler bits are counted as logical zeros.
const auto is_msg_equal = [](uint8_t a, uint8_t b) {
  return ((a == b) || ((a == 0) && (b == srsgnb::ldpc::FILLER_BIT)));
};

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
  std::unique_ptr<srsgnb::ldpc_encoder> enc_generic = srsgnb::create_ldpc_encoder("generic");
  std::unique_ptr<srsgnb::ldpc_decoder> dec_generic = srsgnb::create_ldpc_decoder("generic");

  for (const auto& test_data : ldpc_encoder_test_data) {
    base_graph_t   bg = static_cast<base_graph_t>(test_data.bg - 1);
    lifting_size_t ls = static_cast<lifting_size_t>(test_data.ls);

    // Set base-graph message and codeblock lengths.
    unsigned min_cb_length_bg = 24;
    unsigned max_cb_length_bg = 66;
    unsigned msg_length_bg    = 22;
    if (bg == srsgnb::ldpc::base_graph_t::BG2) {
      min_cb_length_bg = 12;
      max_cb_length_bg = 50;
      msg_length_bg    = 10;
    }

    // Compute lifted messages and codeblock lengths.
    unsigned min_cb_length = min_cb_length_bg * ls;
    unsigned max_cb_length = max_cb_length_bg * ls;
    unsigned msg_length    = msg_length_bg * ls;

    // Read messages and codeblocks.
    const std::vector<uint8_t> messages   = test_data.messages.read();
    const std::vector<uint8_t> codeblocks = test_data.codeblocks.read();

    const unsigned nof_messages = test_data.nof_messages;
    assert(messages.size() == static_cast<size_t>(nof_messages * msg_length));
    assert(codeblocks.size() == static_cast<size_t>(nof_messages * max_cb_length));

    srsgnb::codeblock_metadata::tb_common_metadata cfg_enc{bg, ls};
    srsgnb::ldpc_decoder::config_t                 cfg_dec{bg, ls};

    unsigned used_msg_bits   = 0;
    unsigned used_cblck_bits = 0;
    for (unsigned message_idx = 0; message_idx != nof_messages; ++message_idx) {
      span<const uint8_t> msg_i = span<const uint8_t>(messages).subspan(used_msg_bits, msg_length);
      used_msg_bits += msg_length;
      span<const uint8_t> cblock_i = span<const uint8_t>(codeblocks).subspan(used_cblck_bits, max_cb_length);
      used_cblck_bits += max_cb_length;

      constexpr int NOF_STEPS   = 3;
      unsigned      length_step = (max_cb_length - min_cb_length) / NOF_STEPS;
      // check several shortened codeblocks
      for (unsigned length = min_cb_length; length < max_cb_length; length += length_step) {
        std::vector<uint8_t> encoded(length);
        enc_generic->encode(encoded, msg_i, cfg_enc);
        TESTASSERT(std::equal(encoded.begin(), encoded.end(), cblock_i.begin()), "Wrong codeblock.");

        std::vector<uint8_t> decoded(msg_length);
        std::vector<int8_t>  llrs(length);
        std::transform(cblock_i.begin(), cblock_i.begin() + length, llrs.begin(), compute_llrs);
        dec_generic->decode(decoded, llrs, cfg_dec);
        TESTASSERT(std::equal(decoded.begin(), decoded.end(), msg_i.begin(), is_msg_equal), "Wrong recovered message.");
      }
      // check full-length codeblock
      std::vector<uint8_t> encoded(max_cb_length);
      enc_generic->encode(encoded, msg_i, cfg_enc);
      TESTASSERT(std::equal(encoded.begin(), encoded.end(), cblock_i.begin()), "Wrong codeblock.");

      std::vector<uint8_t> decoded(msg_length);
      std::vector<int8_t>  llrs(max_cb_length);
      std::transform(cblock_i.begin(), cblock_i.end(), llrs.begin(), compute_llrs);
      dec_generic->decode(decoded, llrs, cfg_dec);
      TESTASSERT(std::equal(decoded.begin(), decoded.end(), msg_i.begin(), is_msg_equal), "Wrong recovered message.");
    }
  }
}
