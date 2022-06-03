/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/pdcch/dci_packing.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;

static std::mt19937       rgen(1234);
static constexpr unsigned nof_repetitions = 100;

static void test_dci_1_0_si_rnti_packing()
{
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> system_information_indicator_dist(0, pow2(1) - 1);

  for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
    for (unsigned N_rb_dl_bwp : {24, 48, 96}) {
      unsigned frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
      std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

      dci_1_0_si_rnti_configuration config = {};
      config.N_rb_dl_bwp                   = N_rb_dl_bwp;
      config.frequency_resource            = frequency_resource_dist(rgen);
      config.time_resource                 = time_resource_dist(rgen);
      config.vrb_to_prb_mapping            = vrb_to_prb_mapping_dist(rgen);
      config.modulation_coding_scheme      = modulation_coding_scheme_dist(rgen);
      config.redundancy_version            = redundancy_version_dist(rgen);
      config.system_information_indicator  = system_information_indicator_dist(rgen);

      dci_payload payload = dci_1_0_si_rnti_pack(config);

      // Generate expected payload.
      static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

      // Frequency domain resource assignment - frequency_resource_nof_bits bits.
      for (unsigned bitpos = 0; bitpos != frequency_resource_nof_bits; ++bitpos) {
        expected.push_back((config.frequency_resource >> (frequency_resource_nof_bits - 1 - bitpos)) & 1U);
      }

      // Time domain resource assignment - 4 bit.
      expected.push_back((config.time_resource >> 3U) & 1U);
      expected.push_back((config.time_resource >> 2U) & 1U);
      expected.push_back((config.time_resource >> 1U) & 1U);
      expected.push_back((config.time_resource >> 0U) & 1U);

      // VRB-to-PRB mapping - 1 bit.
      expected.push_back((config.vrb_to_prb_mapping >> 0U) & 1U);

      // Modulation coding scheme - 5 bits.
      expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

      // Redundancy version - 2 bits.
      expected.push_back((config.redundancy_version >> 1U) & 1U);
      expected.push_back((config.redundancy_version >> 0U) & 1U);

      // System information indicator - 1 bit.
      expected.push_back((config.system_information_indicator >> 0U) & 1U);

      // Reserved bits - 15 bit.
      std::fill_n(std::back_inserter(expected), 15, 0);

      // Assert expected payload.
      TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
    }
  }
}

int main()
{
  test_dci_1_0_si_rnti_packing();

  return 0;
}