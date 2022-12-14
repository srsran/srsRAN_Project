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

// Generate a DCI format 0_0 scrambled by TC-RNTI configuration.
static dci_0_0_tc_rnti_configuration build_dci_0_0_tc_rnti_config(unsigned N_rb_ul_bwp, units::bits payload_size)
{
  dci_0_0_tc_rnti_configuration config = {};

  std::uniform_int_distribution<unsigned> N_ul_hop_dist(1, 2);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> frequency_hopping_flag_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> dl_assignment_index_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> tpc_command_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> pucch_resource_indicator_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> pdsch_harq_fb_timing_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> ul_sul_indicator_presence_dist(0, 1);

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_ul_bwp * (N_rb_ul_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  config.payload_size = payload_size;
  config.N_ul_hop     = N_ul_hop_dist(rgen);

  // The hopping offset is packed with N_ul_hop bits.
  std::uniform_int_distribution<unsigned> hopping_offset_dist(0, pow2(config.N_ul_hop) - 1);

  config.hopping_offset           = hopping_offset_dist(rgen);
  config.N_rb_ul_bwp              = N_rb_ul_bwp;
  config.frequency_resource       = frequency_resource_dist(rgen);
  config.time_resource            = time_resource_dist(rgen);
  config.frequency_hopping_flag   = frequency_hopping_flag_dist(rgen);
  config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
  config.redundancy_version       = redundancy_version_dist(rgen);
  config.tpc_command              = tpc_command_dist(rgen);

  return config;
}

static void test_dci_0_0_tc_rnti_packing(const dci_0_0_tc_rnti_configuration& config, const dci_payload& payload)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 0, indicating an UL DCI format.
  expected.push_back(0x00U);

  unsigned N_ul_hop = 0;
  if (config.frequency_hopping_flag) {
    N_ul_hop = config.N_ul_hop;

    // Frequency hopping offset - N_ul_hop bits.
    for (unsigned bitpos = 0; bitpos != N_ul_hop; ++bitpos) {
      expected.push_back((config.hopping_offset >> (N_ul_hop - 1 - bitpos)) & 1U);
    }
  }

  // Position where the MSB of the frequency domain resource assignment field is located. This is useful to apply
  // truncation, if required.
  uint8_t* freq_resource_begin = expected.end();

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2) - N_ul_hop;
  for (unsigned bitpos = 0; bitpos != frequency_resource_nof_bits; ++bitpos) {
    expected.push_back((config.frequency_resource >> (frequency_resource_nof_bits - 1 - bitpos)) & 1U);
  }

  // Time domain resource assignment - 4 bit.
  expected.push_back((config.time_resource >> 3U) & 1U);
  expected.push_back((config.time_resource >> 2U) & 1U);
  expected.push_back((config.time_resource >> 1U) & 1U);
  expected.push_back((config.time_resource >> 0U) & 1U);

  // Frequency hopping flag - 1 bit.
  expected.push_back(config.frequency_hopping_flag & 1U);

  // Modulation and coding scheme - 5 bits.
  expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

  // New data indicator - 1 bit, reserved.
  std::fill_n(std::back_inserter(expected), 1, 0);

  // Redundancy version - 2 bit.
  expected.push_back((config.redundancy_version >> 1U) & 1U);
  expected.push_back((config.redundancy_version >> 0U) & 1U);

  // HARQ process number - 4 bit, reserved.
  std::fill_n(std::back_inserter(expected), 4, 0);

  // TPC command for scheduled PUCCH - 2 bit.
  expected.push_back((config.tpc_command >> 1U) & 1U);
  expected.push_back((config.tpc_command >> 0U) & 1U);

  // Number of packed bits so far.
  units::bits nof_packed_bits(expected.size());

  // The difference between actual size and aligned DCI size determines the amount of padding or truncation. This is
  // required because DCI format 0_0 scrambled by TC-RNTI is used in a common search space, and it must be aligned to
  // the size of the DCI format 1_0 messages.
  int padd_trunc_incl_ul_sul =
      static_cast<int>(config.payload_size.value()) - static_cast<int>(nof_packed_bits.value());

  if (padd_trunc_incl_ul_sul < 0) {
    // Truncation is applied to the MSB bits of the frequency domain resource assignment field.
    unsigned nof_trunc_bits = -padd_trunc_incl_ul_sul;
    expected.erase(freq_resource_begin, freq_resource_begin + nof_trunc_bits);
  }

  if (padd_trunc_incl_ul_sul > 0) {
    // Apply padding, including the UL/SUL reserved field.
    std::fill_n(std::back_inserter(expected), padd_trunc_incl_ul_sul, 0);
  }

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}

// Generate a DCI format 0_0 scrambled by C-RNTI configuration.
static dci_0_0_c_rnti_configuration build_dci_0_0_c_rnti_config(unsigned N_rb_ul_bwp, units::bits payload_size)
{
  dci_0_0_c_rnti_configuration config = {};

  std::uniform_int_distribution<unsigned> N_ul_hop_dist(1, 2);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> frequency_hopping_flag_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> new_data_indicator_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> harq_process_number_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> dl_assignment_index_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> tpc_command_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> pucch_resource_indicator_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> pdsch_harq_fb_timing_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> ul_sul_indicator_presence_dist(0, 1);

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_ul_bwp * (N_rb_ul_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  config.payload_size = payload_size;
  config.N_ul_hop     = N_ul_hop_dist(rgen);

  // The hopping offset is packed with N_ul_hop bits.
  std::uniform_int_distribution<unsigned> hopping_offset_dist(0, pow2(config.N_ul_hop) - 1);

  config.hopping_offset           = hopping_offset_dist(rgen);
  config.N_rb_ul_bwp              = N_rb_ul_bwp;
  config.frequency_resource       = frequency_resource_dist(rgen);
  config.time_resource            = time_resource_dist(rgen);
  config.frequency_hopping_flag   = frequency_hopping_flag_dist(rgen);
  config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
  config.new_data_indicator       = new_data_indicator_dist(rgen);
  config.redundancy_version       = redundancy_version_dist(rgen);
  config.harq_process_number      = harq_process_number_dist(rgen);
  config.tpc_command              = tpc_command_dist(rgen);

  if (ul_sul_indicator_presence_dist(rgen) != 0) {
    // Add the UL/SUL indicator field.
    std::uniform_int_distribution<unsigned> ul_sul_indicator_dist(0, 1);
    config.ul_sul_indicator.emplace(static_cast<bool>(ul_sul_indicator_dist(rgen)));
  }

  return config;
}

static void test_dci_0_0_c_rnti_packing(const dci_0_0_c_rnti_configuration& config, const dci_payload& payload)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 0, indicating an UL DCI format.
  expected.push_back(0x00U);

  unsigned N_ul_hop = 0;
  if (config.frequency_hopping_flag) {
    N_ul_hop = config.N_ul_hop;

    // Frequency hopping offset - N_ul_hop bits.
    for (unsigned bitpos = 0; bitpos != N_ul_hop; ++bitpos) {
      expected.push_back((config.hopping_offset >> (N_ul_hop - 1 - bitpos)) & 1U);
    }
  }

  // Position where the MSB of the frequency domain resource assignment field is located. This is useful to apply
  // truncation, if required.
  uint8_t* freq_resource_begin = expected.end();

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_ul_bwp * (config.N_rb_ul_bwp + 1) / 2) - N_ul_hop;
  for (unsigned bitpos = 0; bitpos != frequency_resource_nof_bits; ++bitpos) {
    expected.push_back((config.frequency_resource >> (frequency_resource_nof_bits - 1 - bitpos)) & 1U);
  }

  // Time domain resource assignment - 4 bit.
  expected.push_back((config.time_resource >> 3U) & 1U);
  expected.push_back((config.time_resource >> 2U) & 1U);
  expected.push_back((config.time_resource >> 1U) & 1U);
  expected.push_back((config.time_resource >> 0U) & 1U);

  // Frequency hopping flag - 1 bit.
  expected.push_back(config.frequency_hopping_flag & 1U);

  // Modulation and coding scheme - 5 bits.
  expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

  // New data indicator - 1 bit.
  expected.push_back((config.new_data_indicator >> 0U) & 1U);

  // Redundancy version - 2 bit.
  expected.push_back((config.redundancy_version >> 1U) & 1U);
  expected.push_back((config.redundancy_version >> 0U) & 1U);

  // HARQ process number - 4 bit.
  expected.push_back((config.harq_process_number >> 3U) & 1U);
  expected.push_back((config.harq_process_number >> 2U) & 1U);
  expected.push_back((config.harq_process_number >> 1U) & 1U);
  expected.push_back((config.harq_process_number >> 0U) & 1U);

  // TPC command for scheduled PUCCH - 2 bit.
  expected.push_back((config.tpc_command >> 1U) & 1U);
  expected.push_back((config.tpc_command >> 0U) & 1U);

  // Number of packed bits so far.
  units::bits nof_packed_bits(expected.size());

  // The difference between actual size and aligned DCI size determines the amount of padding or truncation. This is
  // required because DCI format 0_0 scrambled by C-RNTI can be used in a common search space (fallback DCI format), and
  // it must be aligned to the size of the DCI format 1_0 messages.
  int padd_trunc_incl_ul_sul =
      static_cast<int>(config.payload_size.value()) - static_cast<int>(nof_packed_bits.value());

  if (padd_trunc_incl_ul_sul < 0) {
    // Truncation is applied to the MSB bits of the frequency domain resource assignment field.
    unsigned nof_trunc_bits = -padd_trunc_incl_ul_sul;
    expected.erase(freq_resource_begin, freq_resource_begin + nof_trunc_bits);
  }

  if (padd_trunc_incl_ul_sul > 0) {
    if (config.ul_sul_indicator.has_value()) {
      // UL/SUL field is present. First, apply padding.
      std::fill_n(std::back_inserter(expected), padd_trunc_incl_ul_sul - 1, 0);
      // DCI payload is terminated with the UL/SUL indicator.
      expected.push_back(static_cast<unsigned>(config.ul_sul_indicator.value()) & 1U);
    } else {
      // UL/SUL field is not present, apply padding only.
      std::fill_n(std::back_inserter(expected), padd_trunc_incl_ul_sul, 0);
    }
  }

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}

// Generate a DCI format 1_0 scrambled by C-RNTI configuration.
static dci_1_0_c_rnti_configuration build_dci_1_0_c_rnti_config(unsigned N_rb_dl_bwp, units::bits payload_size)
{
  dci_1_0_c_rnti_configuration config = {};

  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> new_data_indicator_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> harq_process_number_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> dl_assignment_index_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> tpc_command_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> pucch_resource_indicator_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> pdsch_harq_fb_timing_dist(0, pow2(3) - 1);

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  config.payload_size                   = payload_size;
  config.N_rb_dl_bwp                    = N_rb_dl_bwp;
  config.frequency_resource             = frequency_resource_dist(rgen);
  config.time_resource                  = time_resource_dist(rgen);
  config.vrb_to_prb_mapping             = vrb_to_prb_mapping_dist(rgen);
  config.modulation_coding_scheme       = modulation_coding_scheme_dist(rgen);
  config.new_data_indicator             = new_data_indicator_dist(rgen);
  config.redundancy_version             = redundancy_version_dist(rgen);
  config.harq_process_number            = harq_process_number_dist(rgen);
  config.dl_assignment_index            = dl_assignment_index_dist(rgen);
  config.tpc_command                    = tpc_command_dist(rgen);
  config.pucch_resource_indicator       = pucch_resource_indicator_dist(rgen);
  config.pdsch_harq_fb_timing_indicator = pdsch_harq_fb_timing_dist(rgen);

  return config;
}

static void test_dci_1_0_c_rnti_packing(const dci_1_0_c_rnti_configuration& config, const dci_payload& payload)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 1, indicating a DL DCI format.
  expected.push_back(0x01U);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
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

  // Modulation and coding scheme - 5 bits.
  expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

  // New data indicator - 1 bit.
  expected.push_back((config.new_data_indicator >> 0U) & 1U);

  // Redundancy version - 2 bit.
  expected.push_back((config.redundancy_version >> 1U) & 1U);
  expected.push_back((config.redundancy_version >> 0U) & 1U);

  // HARQ process number - 4 bit.
  expected.push_back((config.harq_process_number >> 3U) & 1U);
  expected.push_back((config.harq_process_number >> 2U) & 1U);
  expected.push_back((config.harq_process_number >> 1U) & 1U);
  expected.push_back((config.harq_process_number >> 0U) & 1U);

  // Downlink assignment index - 2 bit.
  expected.push_back((config.dl_assignment_index >> 1U) & 1U);
  expected.push_back((config.dl_assignment_index >> 0U) & 1U);

  // TPC command for scheduled PUCCH - 2 bit.
  expected.push_back((config.tpc_command >> 1U) & 1U);
  expected.push_back((config.tpc_command >> 0U) & 1U);

  // PUCCH resource indicator - 3 bit.
  expected.push_back((config.pucch_resource_indicator >> 2U) & 1U);
  expected.push_back((config.pucch_resource_indicator >> 1U) & 1U);
  expected.push_back((config.pucch_resource_indicator >> 0U) & 1U);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 2U) & 1U);
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 1U) & 1U);
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 0U) & 1U);

  // Determine the amount of padding, since DCI format 1_0 scrambled by C-RNTI, CS-RNTI or CS-RNTI can be used in a UE
  // Specific search space.
  unsigned nof_packed_bits  = expected.size();
  unsigned nof_padding_bits = config.payload_size.value() - nof_packed_bits;

  // Padding bits.
  std::fill_n(std::back_inserter(expected), nof_padding_bits, 0);

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}
// Generate a DCI format 1_0 scrambled by P-RNTI configuration.
static dci_1_0_p_rnti_configuration
build_dci_1_0_p_rnti_config(unsigned N_rb_dl_bwp, dci_1_0_p_rnti_configuration::payload_info short_messages_indicator)
{
  std::uniform_int_distribution<unsigned> short_messages_dist(0, pow2(8) - 1);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);

  // Even though 2 bits are reserved for the TB scaling field, only the
  // 0b00, 0b01 and 0b10 bit words are mapped to scaling factor values.
  std::uniform_int_distribution<unsigned> tb_scaling_dist(0, pow2(1));

  dci_1_0_p_rnti_configuration config = {};

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  config.N_rb_dl_bwp              = N_rb_dl_bwp;
  config.short_messages_indicator = short_messages_indicator;
  config.short_messages           = short_messages_dist(rgen);
  config.frequency_resource       = frequency_resource_dist(rgen);
  config.time_resource            = time_resource_dist(rgen);
  config.vrb_to_prb_mapping       = vrb_to_prb_mapping_dist(rgen);
  config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
  config.tb_scaling               = tb_scaling_dist(rgen);

  return config;
}

static void test_dci_1_0_p_rnti_packing(const dci_1_0_p_rnti_configuration& config, const dci_payload& payload)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Short messages indicator - 2 bits.
  switch (config.short_messages_indicator) {
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

  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
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

  // No padding, since DCI format 1_0 scrambled by P-RNTI is used in common search space.

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}
// Generate a DCI format 1_0 scrambled by SI-RNTI configuration.
static dci_1_0_si_rnti_configuration build_dci_1_0_si_rnti_config(unsigned N_rb_dl_bwp)
{
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> system_information_indicator_dist(0, pow2(1) - 1);

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  dci_1_0_si_rnti_configuration config = {};

  config.N_rb_dl_bwp                  = N_rb_dl_bwp;
  config.frequency_resource           = frequency_resource_dist(rgen);
  config.time_resource                = time_resource_dist(rgen);
  config.vrb_to_prb_mapping           = vrb_to_prb_mapping_dist(rgen);
  config.modulation_coding_scheme     = modulation_coding_scheme_dist(rgen);
  config.redundancy_version           = redundancy_version_dist(rgen);
  config.system_information_indicator = system_information_indicator_dist(rgen);

  return config;
}

static void test_dci_1_0_si_rnti_packing(const dci_1_0_si_rnti_configuration& config, const dci_payload& payload)
{
  // Generate expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
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

  // No padding, since DCI format 1_0 scrambled by SI-RNTI is used in common search space.

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}

// Generate a DCI format 1_0 scrambled by RA-RNTI configuration.
static dci_1_0_ra_rnti_configuration build_dci_1_0_ra_rnti_config(unsigned N_rb_dl_bwp)
{
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);

  // Even though 2 bits are reserved for the TB scaling field, only the
  // 0b00, 0b01 and 0b10 bit words are mapped to scaling factor values.
  std::uniform_int_distribution<unsigned> tb_scaling_dist(0, pow2(1));

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  dci_1_0_ra_rnti_configuration config = {};
  config.N_rb_dl_bwp                   = N_rb_dl_bwp;
  config.frequency_resource            = frequency_resource_dist(rgen);
  config.time_resource                 = time_resource_dist(rgen);
  config.vrb_to_prb_mapping            = vrb_to_prb_mapping_dist(rgen);
  config.modulation_coding_scheme      = modulation_coding_scheme_dist(rgen);
  config.tb_scaling                    = tb_scaling_dist(rgen);

  return config;
}

static void test_dci_1_0_ra_rnti_packing(const dci_1_0_ra_rnti_configuration& config, const dci_payload& payload)
{
  // Generate expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
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

  // No padding, since DCI format 1_0 scrambled by RA-RNTI is used in common search space.

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}

// Generate a DCI format 1_0 scrambled by TC-RNTI configuration.
static dci_1_0_tc_rnti_configuration build_dci_1_0_tc_rnti_config(unsigned N_rb_dl_bwp)
{
  dci_1_0_tc_rnti_configuration config = {};

  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> vrb_to_prb_mapping_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(5) - 1);
  std::uniform_int_distribution<unsigned> new_data_indicator_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> redundancy_version_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> harq_process_number_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> tpc_command_dist(0, pow2(2) - 1);
  std::uniform_int_distribution<unsigned> pucch_resource_indicator_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> pdsch_harq_fb_timing_dist(0, pow2(3) - 1);

  unsigned                                frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

  config.N_rb_dl_bwp                    = N_rb_dl_bwp;
  config.frequency_resource             = frequency_resource_dist(rgen);
  config.time_resource                  = time_resource_dist(rgen);
  config.vrb_to_prb_mapping             = vrb_to_prb_mapping_dist(rgen);
  config.modulation_coding_scheme       = modulation_coding_scheme_dist(rgen);
  config.new_data_indicator             = new_data_indicator_dist(rgen);
  config.redundancy_version             = redundancy_version_dist(rgen);
  config.harq_process_number            = harq_process_number_dist(rgen);
  config.tpc_command                    = tpc_command_dist(rgen);
  config.pucch_resource_indicator       = pucch_resource_indicator_dist(rgen);
  config.pdsch_harq_fb_timing_indicator = pdsch_harq_fb_timing_dist(rgen);

  return config;
}

static void test_dci_1_0_tc_rnti_packing(const dci_1_0_tc_rnti_configuration& config, const dci_payload& payload)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 1, indicating a DL DCI format.
  expected.push_back(0x01U);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
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

  // Modulation and coding scheme - 5 bits.
  expected.push_back((config.modulation_coding_scheme >> 4U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 3U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 2U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 1U) & 1U);
  expected.push_back((config.modulation_coding_scheme >> 0U) & 1U);

  // New data indicator - 1 bit.
  expected.push_back((config.new_data_indicator >> 0U) & 1U);

  // Redundancy version - 2 bit.
  expected.push_back((config.redundancy_version >> 1U) & 1U);
  expected.push_back((config.redundancy_version >> 0U) & 1U);

  // HARQ process number - 4 bit.
  expected.push_back((config.harq_process_number >> 3U) & 1U);
  expected.push_back((config.harq_process_number >> 2U) & 1U);
  expected.push_back((config.harq_process_number >> 1U) & 1U);
  expected.push_back((config.harq_process_number >> 0U) & 1U);

  // Downlink assignment index - 2 bit, reserved.
  std::fill_n(std::back_inserter(expected), 2, 0);

  // TPC command for scheduled PUCCH - 2 bit.
  expected.push_back((config.tpc_command >> 1U) & 1U);
  expected.push_back((config.tpc_command >> 0U) & 1U);

  // PUCCH resource indicator - 3 bit.
  expected.push_back((config.pucch_resource_indicator >> 2U) & 1U);
  expected.push_back((config.pucch_resource_indicator >> 1U) & 1U);
  expected.push_back((config.pucch_resource_indicator >> 0U) & 1U);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 2U) & 1U);
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 1U) & 1U);
  expected.push_back((config.pdsch_harq_fb_timing_indicator >> 0U) & 1U);

  // No padding, since DCI format 1_0 scrambled by TC-RNTI is used in common search space.

  // Assert expected payload.
  TESTASSERT_EQ(bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>(expected.begin(), expected.end()), payload);
}

static void test_dci_rar_packing()
{
  std::uniform_int_distribution<unsigned> frequency_hopping_flag_dist(0, pow2(1) - 1);
  std::uniform_int_distribution<unsigned> frequency_resource_dist(0, pow2(14) - 1);
  std::uniform_int_distribution<unsigned> time_resource_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> modulation_coding_scheme_dist(0, pow2(4) - 1);
  std::uniform_int_distribution<unsigned> tpc_dist(0, pow2(3) - 1);
  std::uniform_int_distribution<unsigned> csi_request_dist(0, pow2(3) - 1);

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

int main()
{
  for (unsigned rep = 0; rep != nof_repetitions; ++rep) {
    for (unsigned dl_bwp_initial_bw : {12U, 96U, MAX_RB}) {
      for (unsigned dl_bwp_active_bw : {12U, 96U, MAX_RB}) {
        for (unsigned ul_bwp_initial_bw : {12U, 96U, MAX_RB}) {
          for (unsigned ul_bwp_active_bw : {12U, 96U, MAX_RB}) {
            dci_size_config config = {};

            // Set the Bandwidth of the UL and DL, active and initial BWP.
            config.ul_bwp_initial_bw = ul_bwp_initial_bw;
            config.ul_bwp_active_bw  = ul_bwp_active_bw;
            config.dl_bwp_initial_bw = dl_bwp_initial_bw;
            config.dl_bwp_active_bw  = dl_bwp_active_bw;

            // Perform the DCI size alignment procedure.
            dci_sizes aligned_sizes = get_dci_sizes(config);

            // Check that the computed DCI format 0_0 and 1_0 payload sizes match for common and UE-specific search
            // sets.
            TESTASSERT_EQ(aligned_sizes.format0_0_common_size, aligned_sizes.format1_0_common_size);
            TESTASSERT_EQ(aligned_sizes.format0_0_ue_size, aligned_sizes.format0_0_ue_size);

            // Generate DCI format 0_0 scrambled by TC-RNTI configuration.
            dci_0_0_tc_rnti_configuration dci0_0_tc_rnti_cfg =
                build_dci_0_0_tc_rnti_config(config.ul_bwp_initial_bw, aligned_sizes.format0_0_common_size);

            // Generate the DCI format 0_0 scrambled by TC-RNTI payload.
            dci_payload dci0_0_tc_rnti_payload = dci_0_0_tc_rnti_pack(dci0_0_tc_rnti_cfg);

            // Test DCI format 0_0 scrambled by TC-RNTI packing.
            test_dci_0_0_tc_rnti_packing(dci0_0_tc_rnti_cfg, dci0_0_tc_rnti_payload);

            // Check DCI payload size.
            TESTASSERT_EQ(aligned_sizes.format0_0_common_size.value(), dci0_0_tc_rnti_payload.size());

            // Generate DCI format 0_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a common
            // search space.
            dci_0_0_c_rnti_configuration dci0_0_c_rnti_cfg_common =
                build_dci_0_0_c_rnti_config(config.ul_bwp_initial_bw, aligned_sizes.format0_0_common_size);

            // Generate DCI format 0_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
            // UE-specific search space.
            dci_0_0_c_rnti_configuration dci0_0_c_rnti_cfg_ue =
                build_dci_0_0_c_rnti_config(config.ul_bwp_active_bw, aligned_sizes.format0_0_ue_size);

            // Generate the DCI format 0_0 scrambled by C-RNTI payloads for common and UE-specific search space sets.
            dci_payload dci0_0_c_rnti_payload_common = dci_0_0_c_rnti_pack(dci0_0_c_rnti_cfg_common);
            dci_payload dci0_0_c_rnti_payload_ue     = dci_0_0_c_rnti_pack(dci0_0_c_rnti_cfg_ue);

            // Test DCI format 0_0 scrambled by C-RNTI packing.
            test_dci_0_0_c_rnti_packing(dci0_0_c_rnti_cfg_common, dci0_0_c_rnti_payload_common);
            test_dci_0_0_c_rnti_packing(dci0_0_c_rnti_cfg_ue, dci0_0_c_rnti_payload_ue);

            // Check DCI payload sizes.
            TESTASSERT_EQ(aligned_sizes.format0_0_common_size.value(), dci0_0_c_rnti_payload_common.size());
            TESTASSERT_EQ(aligned_sizes.format0_0_ue_size.value(), dci0_0_c_rnti_payload_ue.size());

            // Generate DCI format 1_0 scrambled by TC-RNTI configuration.
            dci_1_0_tc_rnti_configuration dci1_0_tc_rnti_cfg = build_dci_1_0_tc_rnti_config(config.dl_bwp_initial_bw);

            // Generate the DCI format 1_0 scrambled by TC-RNTI payload.
            dci_payload dci1_0_tc_rnti_payload = dci_1_0_tc_rnti_pack(dci1_0_tc_rnti_cfg);

            // Test DCI format 1_0 scrambled by TC-RNTI packing.
            test_dci_1_0_tc_rnti_packing(dci1_0_tc_rnti_cfg, dci1_0_tc_rnti_payload);

            // Check DCI payload size.
            TESTASSERT_EQ(aligned_sizes.format1_0_common_size.value(), dci1_0_tc_rnti_payload.size());

            // Generate DCI format 1_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
            // UE-specific search space.
            dci_1_0_c_rnti_configuration dci1_0_c_rnti_cfg_ue =
                build_dci_1_0_c_rnti_config(config.dl_bwp_active_bw, aligned_sizes.format1_0_ue_size);

            // Generate DCI format 1_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
            // common search space.
            dci_1_0_c_rnti_configuration dci1_0_c_rnti_cfg_common =
                build_dci_1_0_c_rnti_config(config.dl_bwp_initial_bw, aligned_sizes.format1_0_common_size);

            // Generate the DCI format 1_0 scrambled by C-RNTI payloads for common and UE-specific search space sets.
            dci_payload dci1_0_c_rnti_payload_common = dci_1_0_c_rnti_pack(dci1_0_c_rnti_cfg_common);
            dci_payload dci1_0_c_rnti_payload_ue     = dci_1_0_c_rnti_pack(dci1_0_c_rnti_cfg_ue);

            // Test DCI format 1_0 scrambled by C-RNTI packing.
            test_dci_1_0_c_rnti_packing(dci1_0_c_rnti_cfg_common, dci1_0_c_rnti_payload_common);
            test_dci_1_0_c_rnti_packing(dci1_0_c_rnti_cfg_ue, dci1_0_c_rnti_payload_ue);

            // Check DCI payload sizes.
            TESTASSERT_EQ(aligned_sizes.format1_0_common_size.value(), dci1_0_c_rnti_payload_common.size());
            TESTASSERT_EQ(aligned_sizes.format1_0_ue_size.value(), dci1_0_c_rnti_payload_ue.size());

            for (dci_1_0_p_rnti_configuration::payload_info short_messages_indicator :
                 {dci_1_0_p_rnti_configuration::payload_info::scheduling_information,
                  dci_1_0_p_rnti_configuration::payload_info::short_messages,
                  dci_1_0_p_rnti_configuration::payload_info::both}) {
              // Generate the DCI format 1_0 scrambled by P-RNTI configuration.
              dci_1_0_p_rnti_configuration dci1_0_p_rnti_cfg =
                  build_dci_1_0_p_rnti_config(config.dl_bwp_initial_bw, short_messages_indicator);

              // Generate the DCI format 1_0 scrambled by P-RNTI payload.
              dci_payload dci1_0_p_rnti_payload = dci_1_0_p_rnti_pack(dci1_0_p_rnti_cfg);

              // Test DCI format 1_0 scrambled by P-RNTI packing.
              test_dci_1_0_p_rnti_packing(dci1_0_p_rnti_cfg, dci1_0_p_rnti_payload);

              // Check DCI payload size.
              TESTASSERT_EQ(aligned_sizes.format1_0_common_size.value(), dci1_0_p_rnti_payload.size());
            }

            // Generate the DCI format 1_0 scrambled by SI-RNTI configuration.
            dci_1_0_si_rnti_configuration dci1_0_si_rnti_cfg = build_dci_1_0_si_rnti_config(config.dl_bwp_initial_bw);

            // Generate the DCI format 1_0 scrambled by SI-RNTI payload.
            dci_payload dci1_0_si_rnti_payload = dci_1_0_si_rnti_pack(dci1_0_si_rnti_cfg);

            // Test DCI format 1_0 scrambled by SI-RNTI packing.
            test_dci_1_0_si_rnti_packing(dci1_0_si_rnti_cfg, dci1_0_si_rnti_payload);

            // Check DCI payload size.
            TESTASSERT_EQ(aligned_sizes.format1_0_common_size.value(), dci1_0_si_rnti_payload.size());

            // Generate the DCI format 1_0 scrambled by RA-RNTI configuration.
            dci_1_0_ra_rnti_configuration dci1_0_ra_rnti_cfg = build_dci_1_0_ra_rnti_config(config.dl_bwp_initial_bw);

            // Generate the DCI format 1_0 scrambled by RA-RNTI payload.
            dci_payload dci1_0_ra_rnti_payload = dci_1_0_ra_rnti_pack(dci1_0_ra_rnti_cfg);

            // Test DCI format 1_0 scrambled by RA-RNTI packing.
            test_dci_1_0_ra_rnti_packing(dci1_0_ra_rnti_cfg, dci1_0_ra_rnti_payload);

            // Check DCI payload size.
            TESTASSERT_EQ(aligned_sizes.format1_0_common_size.value(), dci1_0_ra_rnti_payload.size());
          }
        }
      }
    }
    // Test DCI RAR packing.
    test_dci_rar_packing();
  }
  return 0;
}
