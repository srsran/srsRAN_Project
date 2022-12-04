/*
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
#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>
#include <string>

using namespace srsgnb;
using namespace srsgnb::ldpc;

namespace srsgnb {
std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  return os << fmt::format("BG{}, LS{}", tc.bg, tc.ls);
}
} // namespace srsgnb

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
    if (!dec_factory_generic) {
      dec_factory_generic = create_ldpc_decoder_factory_sw("generic");
      ASSERT_NE(dec_factory_generic, nullptr);
    }
    if (!dec_factory_avx2) {
      dec_factory_avx2 = create_ldpc_decoder_factory_sw("avx2");
      ASSERT_NE(dec_factory_avx2, nullptr);
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
    if (bg == srsgnb::ldpc_base_graph_type::BG2) {
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
    ASSERT_NE(dec_factory_generic, nullptr);

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
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_generic;
  static std::shared_ptr<ldpc_decoder_factory> dec_factory_avx2;

  std::unique_ptr<ldpc_encoder>         encoder_test;
  std::unique_ptr<srsgnb::ldpc_decoder> decoder_test;

  std::vector<uint8_t> messages;
  unsigned             nof_messages;
  unsigned             msg_length;

  std::vector<uint8_t> codeblocks;
  unsigned             min_cb_length;
  unsigned             max_cb_length;

  srsgnb::codeblock_metadata::tb_common_metadata cfg_enc;
  srsgnb::ldpc_decoder::configuration            cfg_dec;
};

std::shared_ptr<ldpc_encoder_factory> LDPCEncDecFixture::enc_factory_generic = nullptr;
std::shared_ptr<ldpc_encoder_factory> LDPCEncDecFixture::enc_factory_avx2    = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_generic = nullptr;
std::shared_ptr<ldpc_decoder_factory> LDPCEncDecFixture::dec_factory_avx2    = nullptr;

// Returns a vector of with values [min_val, min_val + delta, ..., min_val + N * delta, max_val], where N = steps - 1 if
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

TEST_P(LDPCEncDecFixture, LDPCEncDecTest)
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
      // Check the encoder.
      std::vector<uint8_t> encoded(length);
      encoder_test->encode(encoded, msg_i, cfg_enc);
      EXPECT_TRUE(std::equal(encoded.begin(), encoded.end(), cblock_i.begin())) << "Wrong codeblock.";

      // Check the decoder - we need to transform hard bits into soft bits.
      std::vector<uint8_t>              decoded(msg_length);
      std::vector<log_likelihood_ratio> llrs(length);
      std::transform(cblock_i.begin(), cblock_i.begin() + length, llrs.begin(), compute_llrs);
      decoder_test->decode(decoded, llrs, nullptr, cfg_dec);
      EXPECT_TRUE(std::equal(decoded.begin(), decoded.end(), msg_i.begin(), is_msg_equal))
          << "Wrong recovered message.";
    }
  }
}

INSTANTIATE_TEST_SUITE_P(LDPCEncDecSuite,
                         LDPCEncDecFixture,
                         ::testing::Combine(::testing::Values("generic"
#ifdef HAVE_AVX2
                                                              ,
                                                              "avx2"
#endif
                                                              ),
                                            ::testing::ValuesIn(ldpc_encoder_test_data)));
} // namespace
