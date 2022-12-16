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
/// \brief LDPC rate matching and recovering unit test.
///
/// Test vectors are provided that contain full-length codeblocks and the corresponding rate-matched ones, together with
/// all rate matcher configuration parameters. The test compares the examples with internally generated versions of the
/// codeblocks (for both rate-matching and rate-dematching). The test vectors cover a sufficiently wide range of
/// configuration parameters.

#include "ldpc_rate_matcher_test_data.h"
#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srsgnb::ldpc;

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, modulation_scheme mod)
{
  switch (mod) {
    case modulation_scheme::PI_2_BPSK:
      return os << "PI/2 BPSK";
    case modulation_scheme::BPSK:
      return os << "BPSK";
    case modulation_scheme::QPSK:
      return os << "QPSK";
    case modulation_scheme::QAM16:
      return os << "16QAM";
    case modulation_scheme::QAM64:
      return os << "64QAM";
    case modulation_scheme::QAM256:
    default:
      return os << "256QAM";
  }
}

std::ostream& operator<<(std::ostream& os, const test_case_t& tct)
{
  fmt::print(os,
             "RM length {}, RV {}, Mod. {}, LBRM length {}, LBRM {}, Filler bits {}",
             tct.rm_length,
             tct.rv,
             tct.mod_scheme,
             tct.n_ref,
             tct.is_lbrm,
             tct.nof_filler);
  return os;
}

std::ostream& operator<<(std::ostream& os, const span<const uint8_t>& data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsgnb

namespace {

using LDPCRateMatchingParams = test_case_t;

class LDPCRateMatchingFixture : public ::testing::TestWithParam<LDPCRateMatchingParams>
{
protected:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
    if (!rate_dematcher_factory) {
      rate_dematcher_factory = create_ldpc_rate_dematcher_factory_sw();
      ASSERT_NE(rate_dematcher_factory, nullptr) << "Cannot create dematcher factory.";
    }

    if (!rate_matcher_factory) {
      rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
      ASSERT_NE(rate_matcher_factory, nullptr) << "Cannot create matcher factory.";
    }
  }

  // Asserts the creation of rate matcher and dematcher objects.
  void SetUp() override
  {
    matcher = rate_matcher_factory->create();
    ASSERT_NE(matcher, nullptr) << "Cannot create rate matcher.";

    dematcher = rate_dematcher_factory->create();
    ASSERT_NE(dematcher, nullptr) << "Cannot create rate dematcher.";
  }

  static std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory;
  static std::shared_ptr<ldpc_rate_matcher_factory>   rate_matcher_factory;

  std::unique_ptr<ldpc_rate_matcher>   matcher;
  std::unique_ptr<ldpc_rate_dematcher> dematcher;
};

std::shared_ptr<ldpc_rate_dematcher_factory> LDPCRateMatchingFixture::rate_dematcher_factory = nullptr;
std::shared_ptr<ldpc_rate_matcher_factory>   LDPCRateMatchingFixture::rate_matcher_factory   = nullptr;

TEST_P(LDPCRateMatchingFixture, LDPCRateMatchingTest)
{
  LDPCRateMatchingParams test_data = GetParam();
  modulation_scheme      mod       = test_data.mod_scheme;
  unsigned               rm_length = test_data.rm_length;
  std::vector<uint8_t>   matched(rm_length);
  unsigned               n_ref = test_data.is_lbrm ? test_data.n_ref : 0;

  std::vector<uint8_t>                   codeblock = test_data.full_cblock.read();
  codeblock_metadata::tb_common_metadata rm_cfg    = {};
  rm_cfg.rv                                        = test_data.rv;
  rm_cfg.mod                                       = mod;
  rm_cfg.Nref                                      = n_ref;

  matcher->rate_match(matched, codeblock, rm_cfg);

  // Compare the rate matched codeblocks with the benchmark ones.
  std::vector<uint8_t> matched_bm = test_data.rm_cblock.read();
  ASSERT_EQ(matched_bm.size(), rm_length);
  // EXPECT_TRUE(std::equal(matched.cbegin(), matched.cend(), matched_bm.cbegin())) << "Wrong rate matching.";
  EXPECT_EQ(matched, matched_bm) << "Wrong rate matching.";

  // Transform rate-matched bits into log-likelihood ratios.
  std::vector<log_likelihood_ratio> llrs(rm_length);
  auto                              bit_to_llrs = [](const uint8_t& b) { return 1 - 2 * b; };
  std::transform(matched_bm.cbegin(), matched_bm.cend(), llrs.begin(), bit_to_llrs);

  std::vector<log_likelihood_ratio> dematched(codeblock.size());
  unsigned                          nof_filler_bits = test_data.nof_filler;

  codeblock_metadata rdm_cfg          = {rm_cfg, {}};
  rdm_cfg.cb_specific.nof_filler_bits = nof_filler_bits;
  dematcher->rate_dematch(dematched, llrs, true, rdm_cfg);

  // To check the dematcher output, we need to apply the rate matcher to it and compare with the output
  // obtained in the first part of the test. First, transform LLRs into hard bits.
  auto llrs_to_bit = [](log_likelihood_ratio b) {
    if (b == LLR_INFINITY) {
      return FILLER_BIT;
    }
    return b.to_hard_bit();
  };
  std::vector<uint8_t> hard(dematched.size());
  std::transform(dematched.cbegin(), dematched.cend(), hard.begin(), llrs_to_bit);

  // Now, apply the rate matcher and compare results.
  std::vector<uint8_t> matched2(rm_length);
  matcher->rate_match(matched2, hard, rm_cfg);
  EXPECT_EQ(span<const uint8_t>(matched2), span<const uint8_t>(matched)) << "Wrong rate dematching.";
}

INSTANTIATE_TEST_SUITE_P(LDPCRateMatchingSuite,
                         LDPCRateMatchingFixture,
                         ::testing::ValuesIn(ldpc_rate_matcher_test_data));

} // namespace
