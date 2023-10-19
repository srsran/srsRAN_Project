/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
/// \brief Unit test for LDPC encoder and decoder.
///
/// For all possible base graphs and lifting sizes, the test extracts from a file a small set of messages and
/// corresponding codeblocks. The messages are fed to the encoder, whose output is compared to the codeblocks.
/// Similarly, the codeblocks are fed to the decoder and the resulting messages are compared to the example ones.

#include "ldpc_encoder_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/zero.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::ldpc;

namespace srsran {
std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  return os << fmt::format("BG{}, LS{}", tc.bg, tc.ls);
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

bool operator==(span<const uint8_t> lhs, span<const uint8_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace srsran

namespace {

// Fixed log-likelihood ratio amplitude.
constexpr log_likelihood_ratio LLRS_AMPL = 10;
// Transforms hard bits into log-likelihood ratios (with fixed amplitude).
const auto compute_llrs = [](uint8_t b) {
  return ((b == ldpc::FILLER_BIT) ? LLRS_AMPL : log_likelihood_ratio::copysign(LLRS_AMPL, 1 - 2 * b));
};

// Checks whether two messages are equal: filler bits are counted as logical zeros.
const auto is_msg_equal = [](uint8_t a, uint8_t b) { return ((a == b) || ((a == 0) && (b == FILLER_BIT))); };

using LDPCEncDecParams = std::tuple<std::string, test_case_t>;

class LDPCEncDecFixture : public ::testing::TestWithParam<LDPCEncDecParams>
{
protected:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
    if (!enc_factory_generic) {
      enc_factory_generic = create_ldpc_encoder_factory_sw("generic");
      ASSERT_NE(enc_factory_generic, nullptr);
    }
    if (!enc_factory_avx2) {
      enc_factory_avx2 = create_ldpc_encoder_factory_sw("avx2");
      ASSERT_NE(enc_factory_avx2, nullptr);
    }
    if (!enc_factory_neon) {
      enc_factory_neon = create_ldpc_encoder_factory_sw("neon");
      ASSERT_NE(enc_factory_neon, nullptr);
    }
    if (!dec_factory_generic) {
      dec_factory_generic = create_ldpc_decoder_factory_sw("generic");
      ASSERT_NE(dec_factory_generic, nullptr);
    }
    if (!dec_factory_avx2) {
      dec_factory_avx2 = create_ldpc_decoder_factory_sw("avx2");
      ASSERT_NE(dec_factory_avx2, nullptr);
    }
    if (!dec_factory_avx512) {
      dec_factory_avx512 = create_ldpc_decoder_factory_sw("avx512");
      ASSERT_NE(dec_factory_avx512, nullptr);
    }
    if (!dec_factory_neon) {
      dec_factory_neon = create_ldpc_decoder_factory_sw("neon");
      ASSERT_NE(dec_factory_neon, nullptr);
    }
  }

  // Carries out most of the parametrized test setup: computes useful quantities (e.g., message and codeblock lengths),
  // reads the data file and fills encoder/decoder configuration structures.
  LDPCEncDecFixture()
  {
    test_case_t test_data = std::get<1>(GetParam());

    ldpc_base_graph_type bg = static_cast<ldpc_base_graph_type>(test_data.bg);
    lifting_size_t       ls = static_cast<lifting_size_t>(test_data.ls);

    // Set base-graph message and codeblock lengths.
    unsigned min_cb_length_bg = 24;
    unsigned max_cb_length_bg = 66;
    unsigned msg_length_bg    = 22;
    if (bg == srsran::ldpc_base_graph_type::BG2) {
      min_cb_length_bg = 12;
      max_cb_length_bg = 50;
      msg_length_bg    = 10;
    }

    // Compute lifted messages and codeblock lengths.
    min_cb_length = min_cb_length_bg * ls;
    max_cb_length = max_cb_length_bg * ls;
    msg_length    = msg_length_bg * ls;

    // Read messages and codeblocks.
    messages   = test_data.messages.read();
    codeblocks = test_data.codeblocks.read();

    // Remove filler bits from messages.
    std::transform(messages.begin(), messages.end(), messages.begin(), [](uint8_t bit) {
      if (bit == ldpc::FILLER_BIT) {
        return uint8_t{0};
      }
      return bit;
    });

    // Remove filler bits from codeblocks.
    std::transform(codeblocks.begin(), codeblocks.end(), codeblocks.begin(), [](uint8_t bit) {
      if (bit == ldpc::FILLER_BIT) {
        return uint8_t{0};
      }
      return bit;
    });

    nof_messages = test_data.nof_messages;

    // Encoder/decoder configurations.
    cfg_enc = {bg, ls};
    cfg_dec = {{cfg_enc}, {}};
    // There is no noise - one decoder iteration should be enough.
    cfg_dec.algorithm_conf.max_iterations = 1;
  }

  // Finalizes the test setup by asserting the creation of encoder and decoder as well as double-checking some of the
  // quantities computed in the constructor.
  void SetUp() override
  {
    // Check the factories again, since ASSERT_* is not fatal in SetUpTestSuite in old googletest releases.
    ASSERT_NE(enc_factory_generic, nullptr);
    ASSERT_NE(enc_factory_avx2, nullptr);
    ASSERT_NE(enc_factory_neon, nullptr);
    ASSERT_NE(dec_factory_generic, nullptr);
    ASSERT_NE(dec_factory_avx2, nullptr);
    ASSERT_NE(dec_factory_avx512, nullptr);
    ASSERT_NE(dec_factory_neon, nullptr);

    std::string implementation = std::get<0>(GetParam());
    if (implementation == "generic") {
      encoder_test = enc_factory_generic->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_generic->create();
      ASSERT_NE(decoder_test, nullptr);
    } else if (implementation == "avx2") {
      encoder_test = enc_factory_avx2->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_avx2->create();
      ASSERT_NE(decoder_test, nullptr);
    } else if (implementation == "avx512") {
      encoder_test = enc_factory_avx2->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_avx512->create();
      ASSERT_NE(decoder_test, nullptr);
    } else if (implementation == "neon") {
      encoder_test = enc_factory_neon->create();
      ASSERT_NE(encoder_test, nullptr);
      decoder_test = dec_factory_neon->create();
      ASSERT_NE(decoder_test, nullptr);
    } else {
      FAIL() << fmt::format("Invalid implementation type {}.", implementation);
    }

    ASSERT_EQ(messages.size(), static_cast<size_t>(nof_messages * msg_length))
        << fmt::format("Expected message total size  {}, actual {}.", nof_messages * msg_length, messages.size());
    ASSERT_EQ(codeblocks.size(), static_cast<size_t>(nof_messages * max_cb_length)) << fmt::format(
        "Expected codeblock total size  {}, actual {}.", nof_messages * max_cb_length, codeblocks.size());
  }

  static std::shared_ptr<ldpc_encoder_factory> enc_factory_generic;
  static std::shared_ptr<ldpc_encoder_factory> enc_factory_avx2;
  static std::shared_ptr<ldpc_encoder_factory> enc_factory_neon;
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_generic;
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_avx2;
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_avx512;
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_neon;

  std::unique_ptr<ldpc_encoder>         encoder_test;
  std::unique_ptr<srsran::ldpc_decoder> decoder_test;

  std::vector<uint8_t> messages;
  unsigned             nof_messages;
  unsigned             msg_length;

  std::vector<uint8_t> codeblocks;
  unsigned             min_cb_length;
  unsigned             max_cb_length;

  srsran::codeblock_metadata::tb_common_metadata cfg_enc;
  srsran::ldpc_decoder::configuration            cfg_dec;
};

std::shared_ptr<ldpc_encoder_factory> LDPCEncDecFixture::enc_factory_generic = nullptr;
std::shared_ptr<ldpc_encoder_factory> LDPCEncDecFixture::enc_factory_avx2    = nullptr;
std::shared_ptr<ldpc_encoder_factory> LDPCEncDecFixture::enc_factory_neon    = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_generic = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_avx2    = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_avx512  = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_neon    = nullptr;

// Returns a vector with values [min_val, min_val + delta, ..., min_val + N * delta, max_val], where N = steps - 1 if
// max_val - min_val is divisible by steps, and N = steps otherwise.
std::vector<unsigned> create_range(unsigned min_val, unsigned max_val, unsigned steps)
{
  if (steps == 0) {
    return {};
  }

  unsigned val_step = (max_val - min_val) / steps;

  std::vector<unsigned> out;
  for (unsigned val = min_val; val < max_val; val += val_step) {
    out.emplace_back(val);
  }
  out.emplace_back(max_val);
  return out;
}

TEST_P(LDPCEncDecFixture, LDPCEncTest)
{
  unsigned used_msg_bits   = 0;
  unsigned used_cblck_bits = 0;
  // For all test message-codeblock pairs...
  for (unsigned message_idx = 0; message_idx != nof_messages; ++message_idx) {
    span<const uint8_t> msg_i = span<const uint8_t>(messages).subspan(used_msg_bits, msg_length);
    used_msg_bits += msg_length;
    span<const uint8_t> cblock_i = span<const uint8_t>(codeblocks).subspan(used_cblck_bits, max_cb_length);
    used_cblck_bits += max_cb_length;

    // Pack input message.
    dynamic_bit_buffer message_packed(msg_length);
    srsvec::bit_pack(message_packed, msg_i);

    // check several shortened codeblocks.
    constexpr unsigned          NOF_STEPS    = 3;
    const std::vector<unsigned> length_steps = create_range(min_cb_length, max_cb_length, NOF_STEPS);
    for (const unsigned length : length_steps) {
      // Select expected encoded data.
      span<const uint8_t> expected_encoded = cblock_i.first(length);

      // Check the encoder.
      std::vector<uint8_t> encoded(length);
      dynamic_bit_buffer   encoded_packed(length);

      encoder_test->encode(encoded_packed, message_packed, cfg_enc);

      srsvec::bit_unpack(encoded, encoded_packed);
      ASSERT_EQ(span<const uint8_t>(encoded), span<const uint8_t>(expected_encoded)) << "Wrong codeblock.";
    }
  }
}

TEST_P(LDPCEncDecFixture, LDPCDecTest)
{
  unsigned used_msg_bits   = 0;
  unsigned used_cblck_bits = 0;
  // For all test message-codeblock pairs...
  for (unsigned message_idx = 0; message_idx != nof_messages; ++message_idx) {
    span<const uint8_t> msg_i = span<const uint8_t>(messages).subspan(used_msg_bits, msg_length);
    used_msg_bits += msg_length;
    span<const uint8_t> cblock_i = span<const uint8_t>(codeblocks).subspan(used_cblck_bits, max_cb_length);
    used_cblck_bits += max_cb_length;

    // check several shortened codeblocks.
    constexpr unsigned          NOF_STEPS    = 3;
    const std::vector<unsigned> length_steps = create_range(min_cb_length, max_cb_length, NOF_STEPS);
    for (const unsigned length : length_steps) {
      // Check the decoder - we need to transform hard bits into soft bits.
      dynamic_bit_buffer                decoded(msg_length);
      std::vector<uint8_t>              decoded_bits(msg_length);
      std::vector<log_likelihood_ratio> llrs(length);
      std::transform(cblock_i.begin(), cblock_i.begin() + length, llrs.begin(), compute_llrs);

      decoder_test->decode(decoded, llrs, nullptr, cfg_dec);

      // Unpack the decoded message.
      srsvec::bit_unpack(decoded_bits, decoded);

      ASSERT_TRUE(std::equal(decoded_bits.begin(), decoded_bits.end(), msg_i.begin(), is_msg_equal))
          << "Wrong recovered message.";
    }
  }
}

bool all_ones(const bit_buffer& bb)
{
  unsigned            n_bytes    = bb.size() / 8;
  span<const uint8_t> full_bytes = bb.get_buffer().first(n_bytes);
  bool                vv = std::all_of(full_bytes.begin(), full_bytes.end(), [](uint8_t a) { return a == 255; });

  unsigned n_bits = bb.size() - n_bytes * 8;
  if (n_bits != 0) {
    for (unsigned i_bit = n_bytes * 8, max_bit = n_bytes * 8 + n_bits; (i_bit != max_bit); ++i_bit) {
      vv = vv && (bb.extract(i_bit, 1) == 1);
    }
  }
  return vv;
}

TEST_P(LDPCEncDecFixture, LDPCDecTestZeroLLR)
{
  // Check that a codeblock with all zero LLR returns message of all ones and an empty output.
  std::vector<log_likelihood_ratio> llrs(max_cb_length);
  srsvec::zero(llrs);
  dynamic_bit_buffer decoded(msg_length);
  optional<unsigned> n_iters = decoder_test->decode(decoded, llrs, nullptr, cfg_dec);
  ASSERT_FALSE(n_iters.has_value()) << "Without CRC calculator, the decoder should not return a number of iteration.";
  ASSERT_TRUE(all_ones(decoded));
}

TEST_P(LDPCEncDecFixture, LDPCDecTestAlmostZeroLLR)
{
  // Check that a codeblock with all zero LLR but a few very small ones returns message of all ones and an empty output.
  std::vector<log_likelihood_ratio> llrs(max_cb_length);
  srsvec::zero(llrs);

  for (unsigned i_bit = min_cb_length + 2; i_bit < max_cb_length; i_bit += 3) {
    llrs[i_bit] = (i_bit % 2 == 0) ? 1 : -1;
  }

  dynamic_bit_buffer decoded(msg_length);
  decoder_test->decode(decoded, llrs, nullptr, cfg_dec);
  ASSERT_TRUE(all_ones(decoded));
}

INSTANTIATE_TEST_SUITE_P(LDPCEncDecSuite,
                         LDPCEncDecFixture,
                         ::testing::Combine(::testing::Values("generic"
#ifdef HAVE_AVX2
                                                              ,
                                                              "avx2"
#endif // HAVE_AVX2
#ifdef HAVE_AVX512
                                                              ,
                                                              "avx512"
#endif // HAVE_AVX512
#ifdef HAVE_NEON
                                                              ,
                                                              "neon"
#endif // HAVE_NEON
                                                              ),
                                            ::testing::ValuesIn(ldpc_encoder_test_data)));
} // namespace
