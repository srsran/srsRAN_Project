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

static void test_dci_1_0_p_rnti_packing()
{
  std::uniform_int_distribution<unsigned> short_messages_dist(0, pow2(8) - 1);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);

  // Even though 2 bits are reserved for the TB scaling field, only the
  // 0b00, 0b01 and 0b10 bit words are mapped to scaling factor values.
  std::uniform_int_distribution<unsigned> tb_scaling_dist(0, pow2(1));

  for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
    for (unsigned N_rb_dl_bwp : {24, 48, 96}) {
      for (dci_1_0_p_rnti_configuration::payload_info short_messages_indicator :
           {dci_1_0_p_rnti_configuration::payload_info::scheduling_information,
            dci_1_0_p_rnti_configuration::payload_info::short_messages,
            dci_1_0_p_rnti_configuration::payload_info::both}) {
        dci_1_0_p_rnti_configuration config = {};

        unsigned frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
        std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

        config.N_rb_dl_bwp              = N_rb_dl_bwp;
        config.short_messages_indicator = short_messages_indicator;
        config.short_messages           = short_messages_dist(rgen);
        config.frequency_resource       = frequency_resource_dist(rgen);
        config.time_resource            = time_resource_dist(rgen);
        config.vrb_to_prb_mapping       = vrb_to_prb_mapping_dist(rgen);
        config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
        config.tb_scaling               = tb_scaling_dist(rgen);

        // Generate the test payload.
        dci_payload payload = dci_1_0_p_rnti_pack(config);

        // Generate the expected payload.
        static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

        // Short messages indicator - 2 bits.
        switch (short_messages_indicator) {
          case dci_1_0_p_rnti_configuration::payload_info::scheduling_information:
            expected.push_back(0U);
            expected.push_back(1U);
            break;
          case dci_1_0_p_rnti_configuration::payload_info::short_messages:
            expected.push_back(1U);
            expected.push_back(0U);
            break;
          case dci_1_0_p_rnti_configuration::payload_info::both:
            expected.push_back(1U);
            expected.push_back(1U);
            break;
        }

        // Short Messages - 8 bits.
        if (config.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::scheduling_information) {
          // If only the scheduling information for paging is carried, this bit field is reserved.
          std::fill_n(std::back_inserter(expected), 8, 0);
        } else {
          // Short Messages - 8 bits.
          expected.push_back((config.short_messages >> 7U) & 1U);
          expected.push_back((config.short_messages >> 6U) & 1U);
          expected.push_back((config.short_messages >> 5U) & 1U);
          expected.push_back((config.short_messages >> 4U) & 1U);
          expected.push_back((config.short_messages >> 3U) & 1U);
          expected.push_back((config.short_messages >> 2U) & 1U);
          expected.push_back((config.short_messages >> 1U) & 1U);
          expected.push_back((config.short_messages >> 0U) & 1U);
        }

        if (config.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::short_messages) {
          // If only the short message is carried, the scheduling information for paging bit fields are reserved.
          std::fill_n(std::back_inserter(expected), frequency_resource_nof_bits + 12, 0);
        } else {
          // Frequency domain resource assignment - frequency_resource_nof_bits bits.
          for (unsigned bitpos = 0; bitpos != frequency_resource_nof_bits; ++bitpos) {
            expected.push_back((config.frequency_resource >> (frequency_resource_nof_bits - 1 - bitpos)) & 1U);
          }

          // Time domain resource assignment - 4 bits.
          expected.push_back((config.time_resource >> 3U) & 1U);
          expected.push_back((config.time_resource >> 2U) & 1U);
          expected.push_back((config.time_resource >> 1U) & 1U);
          expected.push_back((config.time_resource >> 0U) & 1U);

          // VRB-to-PRB mapping - 1 bit.
          expected.push_back((config.vrb_to_prb_mapping >> 0U) & 1U);

          // Modulation and coding scheme - 5 bits.
          expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
          expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
          expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
          expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
          expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

          // Transport Block scaling - 2 bits.
          expected.push_back((config.tb_scaling >> 1U) & 1U);
          expected.push_back((config.tb_scaling >> 0U) & 1U);
        }

        // Reserved bits: 6 bits.
        std::fill_n(std::back_inserter(expected), 6, 0);

        // Assert expected payload.
        TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
      }
    }
  }
}

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

static void test_dci_1_0_ra_rnti_packing()
{
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);

  // Even though 2 bits are reserved for the TB scaling field, only the
  // 0b00, 0b01 and 0b10 bit words are mapped to scaling factor values.
  std::uniform_int_distribution<unsigned> tb_scaling_dist(0, pow2(1));

  for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
    for (unsigned N_rb_dl_bwp : {24, 48, 96}) {
      unsigned frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
      std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

      dci_1_0_ra_rnti_configuration config = {};
      config.N_rb_dl_bwp                   = N_rb_dl_bwp;
      config.frequency_resource            = frequency_resource_dist(rgen);
      config.time_resource                 = time_resource_dist(rgen);
      config.vrb_to_prb_mapping            = vrb_to_prb_mapping_dist(rgen);
      config.modulation_coding_scheme      = modulation_coding_scheme_dist(rgen);
      config.tb_scaling                    = tb_scaling_dist(rgen);

      dci_payload payload = dci_1_0_ra_rnti_pack(config);

      // Generate expected payload.
      static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

      // Frequency domain resource assignment - frequency_resource_nof_bits bits.
      for (unsigned bitpos = 0; bitpos != frequency_resource_nof_bits; ++bitpos) {
        expected.push_back((config.frequency_resource >> (frequency_resource_nof_bits - 1 - bitpos)) & 1U);
      }

      // Time domain resource assignment - 4 bits.
      expected.push_back((config.time_resource >> 3U) & 1U);
      expected.push_back((config.time_resource >> 2U) & 1U);
      expected.push_back((config.time_resource >> 1U) & 1U);
      expected.push_back((config.time_resource >> 0U) & 1U);

      // VRB-to-PRB mapping - 1 bit.
      expected.push_back((config.vrb_to_prb_mapping >> 0U) & 1U);

      // Modulation and coding scheme - 5 bits.
      expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
      expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

      // Transport Block scaling - 2 bits.
      expected.push_back((config.tb_scaling >> 1U) & 1U);
      expected.push_back((config.tb_scaling >> 0U) & 1U);

      // Reserved bits - 16 bits.
      std::fill_n(std::back_inserter(expected), 16, 0);

      // Assert expected payload.
      TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
    }
  }
}

static void test_dci_rar_packing()
{
  std::uniform_int_distribution<unsigned> frequency_hopping_flag_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(14) - 1);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> tpc_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> csi_request_dist(0, pow2(3) - 1);

  for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
    dci_rar_configuration config    = {};
    config.frequency_hopping_flag   = frequency_hopping_flag_dist(rgen);
    config.frequency_resource       = frequency_resource_dist(rgen);
    config.time_resource            = time_resource_dist(rgen);
    config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
    config.tpc                      = tpc_dist(rgen);
    config.csi_request              = csi_request_dist(rgen);

    dci_payload payload = dci_rar_pack(config);

    // Generate expected payload.
    static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

    // Frequency hopping flag - 1 bit.
    expected.push_back((config.frequency_hopping_flag >> 0U) & 1U);

    // PUSCH frequency resource allocation - 14 bits.
    expected.push_back((config.frequency_resource >> 13U) & 1U);
    expected.push_back((config.frequency_resource >> 12U) & 1U);
    expected.push_back((config.frequency_resource >> 11U) & 1U);
    expected.push_back((config.frequency_resource >> 10U) & 1U);
    expected.push_back((config.frequency_resource >> 9U) & 1U);
    expected.push_back((config.frequency_resource >> 8U) & 1U);
    expected.push_back((config.frequency_resource >> 7U) & 1U);
    expected.push_back((config.frequency_resource >> 6U) & 1U);
    expected.push_back((config.frequency_resource >> 5U) & 1U);
    expected.push_back((config.frequency_resource >> 4U) & 1U);
    expected.push_back((config.frequency_resource >> 3U) & 1U);
    expected.push_back((config.frequency_resource >> 2U) & 1U);
    expected.push_back((config.frequency_resource >> 1U) & 1U);
    expected.push_back((config.frequency_resource >> 0U) & 1U);

    // Time domain resource assignment - 4 bits.
    expected.push_back((config.time_resource >> 3U) & 1U);
    expected.push_back((config.time_resource >> 2U) & 1U);
    expected.push_back((config.time_resource >> 1U) & 1U);
    expected.push_back((config.time_resource >> 0U) & 1U);

    // Time domain resource assignment - 4 bits.
    expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
    expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
    expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
    expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

    // Transmission power control for PUSCH - 3 bits.
    expected.push_back((config.tpc >> 2U) & 1U);
    expected.push_back((config.tpc >> 1U) & 1U);
    expected.push_back((config.tpc >> 0U) & 1U);

    // CSI request - 1 bit.
    expected.push_back((config.csi_request >> 0U) & 1U);

    // Assert expected payload.
    TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
  }
}

int main()
{
  test_dci_1_0_p_rnti_packing();
  test_dci_1_0_si_rnti_packing();
  test_dci_1_0_ra_rnti_packing();
  test_dci_rar_packing();

  return 0;
}
