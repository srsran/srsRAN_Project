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
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
  std::shared_ptr<ldpc_rate_dematcher_factory> rate_dematcher_factory = create_ldpc_rate_dematcher_factory_sw();
  TESTASSERT(rate_dematcher_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(rate_matcher_factory);

  // The LDPC rate matcher object used throughout the test.
  const std::unique_ptr<ldpc_rate_matcher> matcher = rate_matcher_factory->create();
  TESTASSERT(matcher);

  // The LDPC rate dematcher object used throughout the test.
  const std::unique_ptr<ldpc_rate_dematcher> dematcher = rate_dematcher_factory->create();
  TESTASSERT(dematcher);

  for (const auto& test_data : ldpc_rate_matcher_test_data) {
    modulation_scheme    mod       = test_data.mod_scheme;
    unsigned             rm_length = test_data.rm_length;
    std::vector<uint8_t> matched(rm_length);
    unsigned             n_ref = test_data.is_lbrm ? test_data.n_ref : 0;

    std::vector<uint8_t>                   codeblock = test_data.full_cblock.read();
    codeblock_metadata::tb_common_metadata rm_cfg    = {};
    rm_cfg.rv                                        = test_data.rv;
    rm_cfg.mod                                       = mod;
    rm_cfg.Nref                                      = n_ref;

    matcher->rate_match(matched, codeblock, rm_cfg);

    // Compare the rate matched codeblocks with the benchmark ones.
    std::vector<uint8_t> matched_bm = test_data.rm_cblock.read();
    assert(matched_bm.size() == rm_length);
    TESTASSERT(std::equal(matched.cbegin(), matched.cend(), matched_bm.cbegin()), "Wrong rate matching.");

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
    std::vector<uint8_t> hard(dematched.size());
    auto                 llrs_to_bit = [](const log_likelihood_ratio& b) {
      if (b == LLR_INFINITY) {
        return FILLER_BIT;
      }
      return b.to_hard_bit();
    };
    std::transform(dematched.cbegin(), dematched.cend(), hard.begin(), llrs_to_bit);

    // Now, apply the rate matcher and compare results.
    std::vector<uint8_t> matched2(rm_length);
    matcher->rate_match(matched2, hard, rm_cfg);
    TESTASSERT(std::equal(matched.cbegin(), matched.cend(), matched2.cbegin()), "Wrong rate dematching.");
  }
}
