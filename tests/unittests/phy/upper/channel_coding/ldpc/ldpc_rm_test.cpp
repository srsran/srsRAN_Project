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

/// \file
/// \brief LDPC rate matching and recovering unit test.
///
/// Test vectors are provided that contain full-length codeblocks and the corresponding rate-matched ones, together with
/// all rate matcher configuration parameters. The test compares the examples with internally generated versions of the
/// codeblocks (for both rate-matching and rate-dematching). The test vectors cover a sufficiently wide range of
/// configuration parameters.

#include "ldpc_rate_matcher_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/cpu_features.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::ldpc;

namespace srsran {

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

} // namespace srsran

namespace {

#ifdef __x86_64__
bool supports_avx2   = cpu_supports_feature(cpu_feature::avx2);
bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512bw) && cpu_supports_feature(cpu_feature::avx512f);
#endif // __x86_64__

using LDPCRateMatchingParams = std::tuple<std::string, test_case_t>;

class LDPCRateMatchingFixture : public ::testing::TestWithParam<LDPCRateMatchingParams>
{
protected:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
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
  }

  static std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  std::unique_ptr<ldpc_rate_matcher>                matcher;
};

std::shared_ptr<ldpc_rate_matcher_factory> LDPCRateMatchingFixture::rate_matcher_factory = nullptr;

TEST_P(LDPCRateMatchingFixture, LDPCRateMatchingTest)
{
  const LDPCRateMatchingParams& params = GetParam();

  // Get the rate dematcher implementation type.
  std::string rate_dematcher_type = std::get<0>(params);

  // Create rate dematcher factory.
  std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory =
      create_ldpc_rate_dematcher_factory_sw(rate_dematcher_type);
  ASSERT_NE(rate_dematcher_factory, nullptr) << "Cannot create dematcher factory.";

  // Create rate dematcher.
  std::unique_ptr<ldpc_rate_dematcher> dematcher = rate_dematcher_factory->create();

#ifdef __x86_64__
  if ((rate_dematcher_type == "avx2") && !supports_avx2) {
    // Check that the factory did not instantiate the unsupported AVX2 implementation and skip the test case.
    ASSERT_EQ(dematcher, nullptr);
    GTEST_SKIP();
  }

  if ((rate_dematcher_type == "avx512") && !supports_avx512) {
    // Check that the factory did not instantiate the unsupported AVX512 implementation and skip the test case.
    ASSERT_EQ(dematcher, nullptr);
    GTEST_SKIP();
  }
#endif // __x86_64__

  // Check that the dematcher is successfully instantiated.
  ASSERT_NE(dematcher, nullptr) << "Cannot create rate dematcher.";

  test_case_t                                    test_data = std::get<1>(params);
  modulation_scheme                              mod       = test_data.mod_scheme;
  unsigned                                       rm_length = test_data.rm_length;
  std::vector<uint8_t>                           matched(rm_length);
  static_bit_buffer<ldpc::MAX_CODEBLOCK_RM_SIZE> matched_packed(rm_length);
  unsigned                                       n_ref           = test_data.is_lbrm ? test_data.n_ref : 0;
  unsigned                                       nof_filler_bits = test_data.nof_filler;

  std::vector<uint8_t> codeblock     = test_data.full_cblock.read();
  codeblock_metadata   rm_cfg        = {};
  rm_cfg.tb_common.rv                = test_data.rv;
  rm_cfg.tb_common.mod               = mod;
  rm_cfg.tb_common.Nref              = n_ref;
  rm_cfg.cb_specific.nof_filler_bits = nof_filler_bits;

  dynamic_bit_buffer codeblock_packed(codeblock.size());
  srsvec::bit_pack(codeblock_packed, codeblock);

  matcher->rate_match(matched_packed, codeblock_packed, rm_cfg);

  // Unpack rate matched.
  srsvec::bit_unpack(matched, matched_packed);

  // Compare the rate matched codeblocks with the expected ones.
  std::vector<uint8_t> expected_matched = test_data.rm_cblock.read();
  ASSERT_EQ(expected_matched.size(), rm_length);
  EXPECT_EQ(span<const uint8_t>(matched), span<const uint8_t>(expected_matched)) << "Wrong rate matching.";

  // Transform rate-matched bits into log-likelihood ratios.
  std::vector<log_likelihood_ratio> llrs(rm_length);
  auto                              bit_to_llrs = [](const uint8_t& b) { return 1 - 2 * b; };
  std::transform(expected_matched.cbegin(), expected_matched.cend(), llrs.begin(), bit_to_llrs);

  std::vector<log_likelihood_ratio> dematched(codeblock.size());

  dematcher->rate_dematch(dematched, llrs, true, rm_cfg);

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

  // Pack hard bits.
  dynamic_bit_buffer hard_packed(dematched.size());
  srsvec::bit_pack(hard_packed, hard);

  // Now, apply the rate matcher and compare results.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_RM_SIZE> matched_packed2(rm_length);
  matcher->rate_match(matched_packed2, hard_packed, rm_cfg);
  EXPECT_EQ(matched_packed, matched_packed2) << "Wrong rate dematching.";
}

INSTANTIATE_TEST_SUITE_P(LDPCRateMatchingSuite,
                         LDPCRateMatchingFixture,
                         ::testing::Combine(::testing::Values("generic"
#ifdef __x86_64__
                                                              ,
                                                              "avx2",
                                                              "avx512"
#endif // __x86_64__
#ifdef HAVE_NEON
                                                              ,
                                                              "neon"
#endif // HAVE_NEON
                                                              ),
                                            ::testing::ValuesIn(ldpc_rate_matcher_test_data)));
} // namespace
