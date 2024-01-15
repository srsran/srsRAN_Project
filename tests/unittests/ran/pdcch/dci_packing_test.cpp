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

#include "srsran/ran/pdcch/dci_packing.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

// Pseudo-random generator.
static std::mt19937 rgen(1234);

namespace {

using uniform_distribution = std::uniform_int_distribution<unsigned>;

using DciFallbackPackingParams = std::tuple<
    // Bandwidth of the initial UL BWP in number of Resource Blocks.
    unsigned,
    // Bandwidth of the active UL BWP in number of Resource Blocks.
    unsigned,
    // Bandwidth of the initial DL BWP in number of Resource Blocks.
    unsigned,
    // Bandwidth of the active DL BWP in number of Resource Blocks.
    unsigned>;

using DciNonFallbackPackingParams = std::tuple<
    // Bandwidth of the initial UL/DL BWP in number of Resource Blocks.
    unsigned,
    // Bandwidth of the active UL/DL BWP in number of Resource Blocks.
    unsigned,
    // PDSCH HARQ-ACK codebook type.
    pdsch_harq_ack_codebook,
    // Number of UL/DL BWP configured by RRC.
    unsigned,
    // Number of UL/DL time domain resources.
    unsigned,
    // Non-codebook based transmission.
    bool,
    // UL PT-RS configured.
    bool,
    // UL/DL resource allocation type.
    resource_allocation>;

class DciFallbackPackingFixture : public ::testing::TestWithParam<DciFallbackPackingParams>
{
protected:
  // Number of test repetitions for each test case.
  static constexpr unsigned nof_repetitions = 100;

  // DCI size configuration parameters.
  dci_size_config dci_config;
  // Aligned DCI payload sizes.
  dci_sizes aligned_sizes;

  // Pseudo random generators to generate content for the DCI Format 0_0 and 1_0 payloads.
  uniform_distribution N_ul_hop_dist{1, 2};
  uniform_distribution time_resource_dist{0, pow2(4) - 1};
  uniform_distribution frequency_hopping_flag_dist{0, pow2(1) - 1};
  uniform_distribution modulation_coding_scheme_dist{0, pow2(5) - 1};
  uniform_distribution redundancy_version_dist{0, pow2(2) - 1};
  uniform_distribution dl_assignment_index_dist{0, pow2(2) - 1};
  uniform_distribution tpc_command_dist{0, pow2(2) - 1};
  uniform_distribution pucch_resource_indicator_dist{0, pow2(3) - 1};
  uniform_distribution pdsch_harq_fb_timing_dist{0, pow2(3) - 1};
  uniform_distribution ul_sul_indicator_presence_dist{0, 1};
  uniform_distribution vrb_to_prb_mapping_dist{0, pow2(1) - 1};
  uniform_distribution new_data_indicator_dist{0, pow2(1) - 1};
  uniform_distribution harq_process_number_dist{0, pow2(4) - 1};
  uniform_distribution short_messages_dist{0, pow2(8) - 1};
  uniform_distribution system_information_indicator_dist{0, pow2(1) - 1};

  // Pseudo random generator for boolean parameters.
  uniform_distribution bool_dist{0, 1};

  // Even though 2 bits are reserved for the TB scaling field, only the
  // 0b00, 0b01 and 0b10 bit words are mapped to scaling factor values.
  uniform_distribution tb_scaling_dist{0, pow2(1)};

  void SetUp() override
  {
    // Set the Bandwidth of the UL and DL, active and initial BWP.
    dci_config.ul_bwp_initial_bw = std::get<0>(GetParam());
    dci_config.ul_bwp_active_bw  = std::get<1>(GetParam());
    dci_config.dl_bwp_initial_bw = std::get<2>(GetParam());
    dci_config.dl_bwp_active_bw  = std::get<3>(GetParam());
    dci_config.coreset0_bw       = static_cast<bool>(bool_dist(rgen)) ? dci_config.dl_bwp_initial_bw / 2 : 0;

    // Generate fallback DCI payloads on Common and UE-specific search spaces.
    dci_config.dci_0_0_and_1_0_ue_ss = true;

    // Do not generate non-fallback DCI payloads.
    dci_config.dci_0_1_and_1_1_ue_ss = false;

    // Supplementary UL has implications on the size alignment procedure that are not currently handled, therefore, it
    // is not currently supported.
    dci_config.sul_configured = false;

    // Perform the DCI size alignment procedure.
    EXPECT_TRUE(validate_dci_size_config(dci_config));
    aligned_sizes = get_dci_sizes(dci_config);

    // Check that the computed DCI Format 0_0 and 1_0 payload sizes match for Common and UE-specific Search Spaces.
    ASSERT_EQ(aligned_sizes.format0_0_common_size.total, aligned_sizes.format1_0_common_size.total);
    ASSERT_EQ(aligned_sizes.format0_0_ue_size.value().total, aligned_sizes.format1_0_ue_size.value().total);
  }

  // Generate a DCI Format 0_0 scrambled by TC-RNTI configuration.
  dci_0_0_tc_rnti_configuration build_dci_0_0_tc_rnti_config(const dci_0_0_size& payload_size)
  {
    dci_0_0_tc_rnti_configuration config = {};
    config.payload_size                  = payload_size;

    uniform_distribution freq_resource_dist(0, pow2(payload_size.frequency_resource.value()) - 1);

    config.N_ul_hop = N_ul_hop_dist(rgen);

    // The hopping offset is packed with N_ul_hop bits.
    uniform_distribution hopping_offset_dist(0, pow2(config.N_ul_hop) - 1);

    config.hopping_offset           = hopping_offset_dist(rgen);
    config.frequency_resource       = freq_resource_dist(rgen);
    config.time_resource            = time_resource_dist(rgen);
    config.frequency_hopping_flag   = frequency_hopping_flag_dist(rgen);
    config.modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
    config.redundancy_version       = redundancy_version_dist(rgen);
    config.tpc_command              = tpc_command_dist(rgen);

    return config;
  }

  // Generate a DCI Format 0_0 scrambled by C-RNTI configuration.
  dci_0_0_c_rnti_configuration build_dci_0_0_c_rnti_config(const dci_0_0_size& payload_size)
  {
    dci_0_0_c_rnti_configuration config = {};

    uniform_distribution frequency_resource_dist(0, pow2(payload_size.frequency_resource.value()) - 1);

    config.payload_size = payload_size;
    config.N_ul_hop     = N_ul_hop_dist(rgen);

    // The hopping offset is packed with N_ul_hop bits.
    uniform_distribution hopping_offset_dist(0, pow2(config.N_ul_hop) - 1);

    config.hopping_offset           = hopping_offset_dist(rgen);
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
      uniform_distribution ul_sul_indicator_dist(0, 1);
      config.ul_sul_indicator.emplace(static_cast<bool>(ul_sul_indicator_dist(rgen)));
    }

    return config;
  }

  // Generate a DCI Format 1_0 scrambled by C-RNTI configuration.
  dci_1_0_c_rnti_configuration build_dci_1_0_c_rnti_config(const dci_1_0_size& payload_size)
  {
    dci_1_0_c_rnti_configuration config = {};

    unsigned             frequency_resource_nof_bits = payload_size.frequency_resource.value();
    uniform_distribution frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

    config.payload_size                   = payload_size;
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

  // Generate a DCI Format 1_0 scrambled by P-RNTI configuration.
  dci_1_0_p_rnti_configuration
  build_dci_1_0_p_rnti_config(unsigned N_rb_dl_bwp, dci_1_0_p_rnti_configuration::payload_info short_messages_indicator)
  {
    dci_1_0_p_rnti_configuration config = {};

    unsigned             frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
    uniform_distribution frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

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

  // Generate a DCI Format 1_0 scrambled by SI-RNTI configuration.
  dci_1_0_si_rnti_configuration build_dci_1_0_si_rnti_config(unsigned N_rb_dl_bwp)
  {
    unsigned             frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
    uniform_distribution frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

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

  // Generate a DCI Format 1_0 scrambled by RA-RNTI configuration.
  dci_1_0_ra_rnti_configuration build_dci_1_0_ra_rnti_config(unsigned N_rb_dl_bwp)
  {
    unsigned             frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
    uniform_distribution frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

    dci_1_0_ra_rnti_configuration config = {};
    config.N_rb_dl_bwp                   = N_rb_dl_bwp;
    config.frequency_resource            = frequency_resource_dist(rgen);
    config.time_resource                 = time_resource_dist(rgen);
    config.vrb_to_prb_mapping            = vrb_to_prb_mapping_dist(rgen);
    config.modulation_coding_scheme      = modulation_coding_scheme_dist(rgen);
    config.tb_scaling                    = tb_scaling_dist(rgen);

    return config;
  }

  // Generate a DCI Format 1_0 scrambled by TC-RNTI configuration.
  dci_1_0_tc_rnti_configuration build_dci_1_0_tc_rnti_config(unsigned N_rb_dl_bwp)
  {
    dci_1_0_tc_rnti_configuration config = {};

    unsigned             frequency_resource_nof_bits = log2_ceil(N_rb_dl_bwp * (N_rb_dl_bwp + 1) / 2);
    uniform_distribution frequency_resource_dist(0, pow2(frequency_resource_nof_bits) - 1);

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
};

class DciNonFallbackPackingFixture : public ::testing::TestWithParam<DciNonFallbackPackingParams>
{
protected:
  // DM-RS mapping type enum, used to map randomly generated integer values to a DM-RS mapping type configuration.
  enum class dmrs_mapping_type : unsigned { type_a = 0, type_b = 1, both = 2 };

  // Number of test repetitions for each test case.
  static constexpr unsigned nof_repetitions = 100;

  // Maximum number of resource block groups per BWP.
  const unsigned MAX_NOF_RBGS = 18;

  // DCI payload size configuration parameters.
  dci_size_config dci_config;

  // Pseudo random generator for boolean parameters.
  uniform_distribution bool_dist{0, 1};

  // Pseudo random generators to generate content for the DCI Format 0_1 and 1_1 payload fields with fixed bit size.
  uniform_distribution modulation_coding_scheme_dist{0, pow2(5) - 1};
  uniform_distribution redundancy_version_dist{0, pow2(2) - 1};
  uniform_distribution tpc_command_dist{0, pow2(2) - 1};
  uniform_distribution pucch_resource_indicator_dist{0, pow2(3) - 1};
  uniform_distribution new_data_indicator_dist{0, pow2(1) - 1};
  uniform_distribution harq_process_number_dist{0, pow2(4) - 1};
  uniform_distribution ul_sch_indicator_dist{0, pow2(1) - 1};
  uniform_distribution pucch_res_indicator_dist{0, pow2(3) - 1};

  // Pseudo random generators to generate content for non-optional DCI size configuration parameters.
  uniform_distribution report_trigger_size_dist{0, 6};
  uniform_distribution nof_aperiodic_zp_csi_dist{0, 3};
  uniform_distribution nof_pdsch_ack_timings_dist{1, 8};
  uniform_distribution nof_srs_resources_dist{1, 4};

  // Pseudo random generators to generate content for the optional DCI size configuration parameters.
  uniform_distribution nof_rb_groups_dist{1, MAX_NOF_RBGS};
  uniform_distribution dmrs_mapping_type_dist{0, 2};
  uniform_distribution dmrs_config_dist{1, 2};
  uniform_distribution dmrs_max_len_dist{1, 2};
  uniform_distribution max_cbg_tb_dist{1, 4};

  dci_sizes generate_dci_sizes()
  {
    // Set all the test case parameters influencing DCI payload size and format.
    dci_config.ul_bwp_initial_bw         = std::get<0>(GetParam());
    dci_config.ul_bwp_active_bw          = std::get<1>(GetParam());
    dci_config.dl_bwp_initial_bw         = std::get<0>(GetParam());
    dci_config.dl_bwp_active_bw          = std::get<1>(GetParam());
    dci_config.pdsch_harq_ack_cb         = std::get<2>(GetParam());
    dci_config.nof_ul_bwp_rrc            = std::get<3>(GetParam());
    dci_config.nof_dl_bwp_rrc            = std::get<3>(GetParam());
    dci_config.nof_ul_time_domain_res    = std::get<4>(GetParam());
    dci_config.nof_dl_time_domain_res    = std::get<4>(GetParam());
    dci_config.tx_config_non_codebook    = std::get<5>(GetParam());
    dci_config.ptrs_uplink_configured    = std::get<6>(GetParam());
    dci_config.pusch_res_allocation_type = std::get<7>(GetParam());
    dci_config.pdsch_res_allocation_type = std::get<7>(GetParam());
    dci_config.coreset0_bw               = static_cast<bool>(bool_dist(rgen)) ? dci_config.dl_bwp_initial_bw / 2 : 0;

    // Supplementary UL has implications on the size alignment procedure that are not currently handled, therefore, it
    // is not currently supported.
    dci_config.sul_configured = false;

    // Generate all possible DCI sizes.
    dci_config.dci_0_0_and_1_0_ue_ss = true;
    dci_config.dci_0_1_and_1_1_ue_ss = true;

    // Randomly set the DCI size parameters that are not in the test case parameters.
    dci_config.cross_carrier_configured     = static_cast<bool>(bool_dist(rgen));
    dci_config.transform_precoding_enabled  = static_cast<bool>(bool_dist(rgen));
    dci_config.report_trigger_size          = report_trigger_size_dist(rgen);
    dci_config.nof_aperiodic_zp_csi         = nof_aperiodic_zp_csi_dist(rgen);
    dci_config.nof_pdsch_ack_timings        = nof_pdsch_ack_timings_dist(rgen);
    dci_config.dynamic_prb_bundling         = static_cast<bool>(bool_dist(rgen));
    dci_config.rm_pattern_group1            = static_cast<bool>(bool_dist(rgen));
    dci_config.rm_pattern_group2            = static_cast<bool>(bool_dist(rgen));
    dci_config.pdsch_two_codewords          = static_cast<bool>(bool_dist(rgen));
    dci_config.pdsch_tci                    = static_cast<bool>(bool_dist(rgen));
    dci_config.cbg_flush_indicator          = static_cast<bool>(bool_dist(rgen));
    dci_config.dynamic_beta_offsets         = static_cast<bool>(bool_dist(rgen));
    dci_config.multiple_scells              = static_cast<bool>(bool_dist(rgen));
    dci_config.frequency_hopping_configured = static_cast<bool>(bool_dist(rgen));

    dci_config.nof_srs_resources = nof_srs_resources_dist(rgen);

    // Number of SRS resources for codebook transmission is up to 2.
    if (!dci_config.tx_config_non_codebook && (dci_config.nof_srs_resources > 2)) {
      dci_config.nof_srs_resources = 2;
    }

    // Clear optional fields that are not always set.
    dci_config.pdsch_dmrs_A_type.reset();
    dci_config.pdsch_dmrs_B_type.reset();
    dci_config.pusch_dmrs_A_type.reset();
    dci_config.pusch_dmrs_B_type.reset();
    dci_config.pdsch_dmrs_A_max_len.reset();
    dci_config.pdsch_dmrs_B_max_len.reset();
    dci_config.pusch_dmrs_A_max_len.reset();
    dci_config.pusch_dmrs_B_max_len.reset();

    // Set optional size parameters to prevent unmet requirements.
    dci_config.dynamic_dual_harq_ack_cb = static_cast<bool>(bool_dist(rgen));

    dci_config.nof_ul_rb_groups       = nof_rb_groups_dist(rgen);
    dci_config.nof_dl_rb_groups       = nof_rb_groups_dist(rgen);
    dci_config.nof_srs_ports          = 1;
    dci_config.pusch_max_layers       = 1;
    dci_config.max_rank               = 1;
    dci_config.ptrs_uplink_configured = false;

    dmrs_mapping_type mapping_type = static_cast<dmrs_mapping_type>(dmrs_mapping_type_dist(rgen));

    // Set the DM-RS type and length for the configured mapping types.
    switch (mapping_type) {
      case dmrs_mapping_type::type_a:
        dci_config.pusch_dmrs_A_type = (dmrs_config_dist(rgen) == 2) && (!dci_config.transform_precoding_enabled)
                                           ? dmrs_config_type::type2
                                           : dmrs_config_type::type1;
        dci_config.pdsch_dmrs_A_type =
            (dmrs_config_dist(rgen) == 1) ? dmrs_config_type::type1 : dmrs_config_type::type2;
        dci_config.pusch_dmrs_A_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        dci_config.pdsch_dmrs_A_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        break;
      case dmrs_mapping_type::type_b:
        dci_config.pusch_dmrs_B_type = (dmrs_config_dist(rgen) == 2) && (!dci_config.transform_precoding_enabled)
                                           ? dmrs_config_type::type2
                                           : dmrs_config_type::type1;
        dci_config.pdsch_dmrs_B_type =
            (dmrs_config_dist(rgen) == 1) ? dmrs_config_type::type1 : dmrs_config_type::type2;
        dci_config.pusch_dmrs_B_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        dci_config.pdsch_dmrs_B_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        break;
      case dmrs_mapping_type::both:
        dci_config.pusch_dmrs_A_type = (dmrs_config_dist(rgen) == 2) && (!dci_config.transform_precoding_enabled)
                                           ? dmrs_config_type::type2
                                           : dmrs_config_type::type1;
        dci_config.pusch_dmrs_B_type = (dmrs_config_dist(rgen) == 2) && (!dci_config.transform_precoding_enabled)
                                           ? dmrs_config_type::type2
                                           : dmrs_config_type::type1;
        dci_config.pdsch_dmrs_A_type =
            (dmrs_config_dist(rgen) == 1) ? dmrs_config_type::type1 : dmrs_config_type::type2;
        dci_config.pusch_dmrs_A_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        dci_config.pdsch_dmrs_A_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        dci_config.pdsch_dmrs_B_type =
            (dmrs_config_dist(rgen) == 1) ? dmrs_config_type::type1 : dmrs_config_type::type2;
        dci_config.pusch_dmrs_B_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        dci_config.pdsch_dmrs_B_max_len =
            (dmrs_max_len_dist(rgen) == 1) ? dmrs_max_length::len1 : dmrs_max_length::len2;
        break;
    }

    dci_config.max_cbg_tb_pusch = 2 * max_cbg_tb_dist(rgen);
    dci_config.max_cbg_tb_pdsch = 2 * max_cbg_tb_dist(rgen);

    dci_config.interleaved_vrb_prb_mapping = static_cast<bool>(bool_dist(rgen));

    // Perform the DCI size alignment procedure.
    EXPECT_TRUE(validate_dci_size_config(dci_config));
    dci_sizes aligned_sizes = get_dci_sizes(dci_config);

    return aligned_sizes;
  }

  // Generate a DCI Format 0_1 configuration.
  dci_0_1_configuration build_dci_0_1_config(const dci_0_1_size& payload_size)
  {
    dci_0_1_configuration config = {};

    // Pseudo random generators for non-optional DCI fields with variable bit size.
    uniform_distribution frequency_res_dist(0, pow2(payload_size.frequency_resource.value()) - 1);
    uniform_distribution time_res_dist{0, pow2(payload_size.time_resource.value()) - 1};
    uniform_distribution first_dl_idx_dist(0, pow2(payload_size.first_dl_assignment_idx.value()) - 1);
    uniform_distribution srs_res_indicator_dist{0, pow2(payload_size.srs_resource_indicator.value()) - 1};
    uniform_distribution antenna_ports_dist{0, pow2(payload_size.antenna_ports.value()) - 1};
    uniform_distribution srs_request_dist{0, pow2(payload_size.srs_request.value()) - 1};

    config.payload_size = payload_size;

    if (payload_size.carrier_indicator.value() != 0) {
      // Set the carrier indicator field.
      uniform_distribution carrier_indicator_dist{0, pow2(payload_size.carrier_indicator.value()) - 1};
      config.carrier_indicator = carrier_indicator_dist(rgen);
    }

    if (payload_size.ul_sul_indicator.value() != 0) {
      // Set the UL/SUL indicator field.
      uniform_distribution ul_sul_indicator_dist(0, pow2(payload_size.ul_sul_indicator.value()) - 1);
      config.ul_sul_indicator = static_cast<bool>(ul_sul_indicator_dist(rgen));
    }

    if (payload_size.bwp_indicator.value() != 0) {
      // Set the BWP indicator field.
      uniform_distribution bwp_indicator_dist(0, pow2(payload_size.bwp_indicator.value()) - 1);
      config.bwp_indicator = bwp_indicator_dist(rgen);
    }

    if (dci_config.pusch_res_allocation_type == resource_allocation::dynamic_switch) {
      // Set the dynamic PUSCH resource allocation type.
      uniform_distribution dyn_res_alloc_type_dist(0, 1);
      config.dynamic_pusch_res_allocation_type = dyn_res_alloc_type_dist(rgen) == 0
                                                     ? dynamic_resource_allocation::type_0
                                                     : dynamic_resource_allocation::type_1;
    }

    if (dci_config.frequency_hopping_configured &&
        (dci_config.pusch_res_allocation_type != resource_allocation::resource_allocation_type_0)) {
      // Set the frequency hopping flag.
      uniform_distribution flag_dist{0, 1};
      config.frequency_hopping_flag = static_cast<bool>(flag_dist(rgen));

      if (config.frequency_hopping_flag) {
        // If the frequency hopping flag is set to true, configure the rest of the frequency hopping parameters.
        uniform_distribution N_ul_hop_dist{1, 2};
        config.N_ul_hop = N_ul_hop_dist(rgen);

        // The hopping offset is packed with N_ul_hop bits.
        uniform_distribution hopping_offset_dist(0, pow2(config.N_ul_hop.value()) - 1);
        config.hopping_offset = hopping_offset_dist(rgen);
      }
    }

    config.frequency_resource        = frequency_res_dist(rgen);
    config.time_resource             = time_res_dist(rgen);
    config.modulation_coding_scheme  = modulation_coding_scheme_dist(rgen);
    config.new_data_indicator        = new_data_indicator_dist(rgen);
    config.redundancy_version        = redundancy_version_dist(rgen);
    config.harq_process_number       = harq_process_number_dist(rgen);
    config.first_dl_assignment_index = first_dl_idx_dist(rgen);

    if (payload_size.second_dl_assignment_idx.value() != 0) {
      // Set the second DL assignment index field.
      uniform_distribution second_dl_idx_dist(0, pow2(payload_size.second_dl_assignment_idx.value()) - 1);
      config.second_dl_assignment_index = second_dl_idx_dist(rgen);
    }

    config.tpc_command            = tpc_command_dist(rgen);
    config.srs_resource_indicator = srs_res_indicator_dist(rgen);

    if (payload_size.precoding_info_nof_layers.value() != 0) {
      // Set the precoding information and number of layers field.
      uniform_distribution precoding_info_nof_layers_dist(0, pow2(payload_size.precoding_info_nof_layers.value()) - 1);
      config.precoding_info_nof_layers = precoding_info_nof_layers_dist(rgen);
    }

    config.antenna_ports = antenna_ports_dist(rgen);
    config.srs_request   = srs_request_dist(rgen);

    if (payload_size.csi_request.value() != 0) {
      // Set the CSI request field.
      uniform_distribution csi_request_dist(0, pow2(payload_size.csi_request.value()) - 1);
      config.csi_request = csi_request_dist(rgen);
    }

    if (payload_size.cbg_transmission_info.value() != 0) {
      // Set the CBG transmission information field.
      uniform_distribution cbg_transmission_info_dist(0, pow2(payload_size.cbg_transmission_info.value()) - 1);
      config.cbg_transmission_info = cbg_transmission_info_dist(rgen);
    }

    if (payload_size.ptrs_dmrs_association.value() != 0) {
      // Set the PT-RS/DM-RS association field.
      uniform_distribution ptrs_dmrs_association_dist(0, pow2(payload_size.ptrs_dmrs_association.value()) - 1);
      config.ptrs_dmrs_association = ptrs_dmrs_association_dist(rgen);
    }

    if (payload_size.beta_offset_indicator.value() != 0) {
      // Set the beta offset indicator field.
      uniform_distribution beta_offset_indicator_dist(0, pow2(payload_size.beta_offset_indicator.value()) - 1);
      config.beta_offset_indicator = beta_offset_indicator_dist(rgen);
    }

    if (payload_size.dmrs_seq_initialization.value() != 0) {
      // Set the DM-RS sequence initialization field.
      uniform_distribution dmrs_seq_initialization_dist(0, pow2(payload_size.dmrs_seq_initialization.value()) - 1);
      config.dmrs_seq_initialization = dmrs_seq_initialization_dist(rgen);
    }

    config.ul_sch_indicator = ul_sch_indicator_dist(rgen);

    return config;
  }

  // Generate a DCI Format 1_1 configuration.
  dci_1_1_configuration build_dci_1_1_config(const dci_1_1_size& payload_size)
  {
    dci_1_1_configuration config = {};

    // Pseudo random generators for non-optional DCI fields with variable bit size.
    uniform_distribution frequency_res_dist(0, pow2(payload_size.frequency_resource.value()) - 1);
    uniform_distribution time_res_dist{0, pow2(payload_size.time_resource.value()) - 1};
    uniform_distribution antenna_ports_dist{0, pow2(payload_size.antenna_ports.value()) - 1};
    uniform_distribution srs_request_dist{0, pow2(payload_size.srs_request.value()) - 1};

    config.payload_size = payload_size;

    if (payload_size.carrier_indicator.value() != 0) {
      // Set the carrier indicator field.
      uniform_distribution carrier_indicator_dist{0, pow2(payload_size.carrier_indicator.value()) - 1};
      config.carrier_indicator = carrier_indicator_dist(rgen);
    }

    if (payload_size.bwp_indicator.value() != 0) {
      // Set the BWP indicator field.
      uniform_distribution bwp_indicator_dist(0, pow2(payload_size.bwp_indicator.value()) - 1);
      config.bwp_indicator = bwp_indicator_dist(rgen);
    }

    if (dci_config.pdsch_res_allocation_type == resource_allocation::dynamic_switch) {
      // Set the dynamic PDSCH resource allocation type.
      uniform_distribution dyn_res_alloc_type_dist(0, 1);
      config.dynamic_pdsch_res_allocation_type = (dyn_res_alloc_type_dist(rgen) == 0)
                                                     ? dynamic_resource_allocation::type_0
                                                     : dynamic_resource_allocation::type_1;
    }

    config.frequency_resource = frequency_res_dist(rgen);
    config.time_resource      = time_res_dist(rgen);

    if (payload_size.vrb_prb_mapping.value() != 0) {
      // Set the VRB-to-PRB mapping field.
      uniform_distribution vrb_prb_mapping_dist(0, pow2(payload_size.vrb_prb_mapping.value()) - 1);
      config.vrb_prb_mapping = vrb_prb_mapping_dist(rgen);
    }

    if (payload_size.prb_bundling_size_indicator.value() != 0) {
      // Set the PRB bundling size indicator field.
      uniform_distribution prb_bundling_size_indicator_dist(0,
                                                            pow2(payload_size.prb_bundling_size_indicator.value()) - 1);
      config.prb_bundling_size_indicator = prb_bundling_size_indicator_dist(rgen);
    }

    if (payload_size.rate_matching_indicator.value() != 0) {
      // Set the rate matching indicator field.
      uniform_distribution rate_matching_indicator_dist(0, pow2(payload_size.rate_matching_indicator.value()) - 1);
      config.rate_matching_indicator = rate_matching_indicator_dist(rgen);
    }

    if (payload_size.zp_csi_rs_trigger.value() != 0) {
      // Set the ZP CSI-RS trigger field.
      uniform_distribution zp_csi_rs_trigger_dist(0, pow2(payload_size.zp_csi_rs_trigger.value()) - 1);
      config.zp_csi_rs_trigger = zp_csi_rs_trigger_dist(rgen);
    }

    config.tb1_modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
    config.tb1_new_data_indicator       = new_data_indicator_dist(rgen);
    config.tb1_redundancy_version       = redundancy_version_dist(rgen);

    if (payload_size.tb2_modulation_coding_scheme.value() != 0) {
      // Set the Modulation and coding scheme for TB 2 field.
      config.tb2_modulation_coding_scheme = modulation_coding_scheme_dist(rgen);
    }

    if (payload_size.tb2_new_data_indicator.value() != 0) {
      // Set the New data indicator for TB 2 field.
      config.tb2_new_data_indicator = new_data_indicator_dist(rgen);
    }

    if (payload_size.tb2_redundancy_version.value() != 0) {
      // Set the Redundancy version for TB 2 field.
      config.tb2_redundancy_version = redundancy_version_dist(rgen);
    }

    config.harq_process_number = harq_process_number_dist(rgen);

    if (payload_size.downlink_assignment_index.value() != 0) {
      // Set the Downlink Assignment Index (DAI) field.
      uniform_distribution downlink_assignment_index_dist(0, pow2(payload_size.downlink_assignment_index.value()) - 1);
      config.downlink_assignment_index = downlink_assignment_index_dist(rgen);
    }

    config.tpc_command              = tpc_command_dist(rgen);
    config.pucch_resource_indicator = pucch_res_indicator_dist(rgen);

    if (payload_size.pdsch_harq_fb_timing_indicator.value() != 0) {
      // Set the PDSCH to HARQ feedback timing indicator field.
      uniform_distribution pdsch_harq_fb_timing_indicator_dist(
          0, pow2(payload_size.pdsch_harq_fb_timing_indicator.value()) - 1);
      config.pdsch_harq_fb_timing_indicator = pdsch_harq_fb_timing_indicator_dist(rgen);
    }

    config.antenna_ports = antenna_ports_dist(rgen);

    if (payload_size.tx_config_indication.value() != 0) {
      // Set the transmission configuration indicator field.
      uniform_distribution tx_config_indication_dist(0, pow2(payload_size.tx_config_indication.value()) - 1);
      config.tx_config_indication = tx_config_indication_dist(rgen);
    }

    config.srs_request = srs_request_dist(rgen);

    if (payload_size.cbg_transmission_info.value() != 0) {
      // Set the CBG transmission information field.
      uniform_distribution cbg_transmission_info_dist(0, pow2(payload_size.cbg_transmission_info.value()) - 1);
      config.cbg_transmission_info = cbg_transmission_info_dist(rgen);
    }

    if (payload_size.cbg_flushing_info.value() != 0) {
      // Set the CBG flushing information field.
      uniform_distribution cbg_flushing_info_dist(0, pow2(payload_size.cbg_flushing_info.value()) - 1);
      config.cbg_flushing_info = cbg_flushing_info_dist(rgen);
    }

    // Set the DM-RS sequence initialization field.
    uniform_distribution dmrs_seq_initialization_dist(0, 1);
    config.dmrs_seq_initialization = dmrs_seq_initialization_dist(rgen);

    return config;
  }
};

class DciRarPackingFixture : public ::testing::Test
{
protected:
  // Number of test repetitions for each test case.
  static constexpr unsigned nof_repetitions = 100;

  uniform_distribution frequency_hopping_flag_dist{0, pow2(1) - 1};
  uniform_distribution frequency_resource_dist{0, pow2(14) - 1};
  uniform_distribution time_resource_dist{0, pow2(4) - 1};
  uniform_distribution modulation_coding_scheme_dist{0, pow2(4) - 1};
  uniform_distribution tpc_dist{0, pow2(3) - 1};
  uniform_distribution csi_request_dist{0, pow2(3) - 1};
};

} // namespace

// Packs a DCI field into an expected DCI payload.
static void expected_pack(static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE>& expected,
                          unsigned                                                       value,
                          unsigned                                                       nof_bits)
{
  for (unsigned bitpos = 0; bitpos != nof_bits; ++bitpos) {
    expected.push_back((value >> (nof_bits - 1 - bitpos)) & 1U);
  }
}

// Builds a DCI Format 0_0 expected payload scrambled by TC-RNTI.
static dci_payload build_dci_0_0_tc_rnti_expected(const dci_0_0_tc_rnti_configuration& config)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 0, indicating an UL DCI format.
  expected.push_back(0x00U);

  unsigned N_ul_hop = 0;
  if (config.frequency_hopping_flag) {
    N_ul_hop = config.N_ul_hop;

    // Frequency hopping offset - N_ul_hop bits.
    expected_pack(expected, config.hopping_offset, N_ul_hop);
  }

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = config.payload_size.frequency_resource.value() - N_ul_hop;
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  expected_pack(expected, config.time_resource, 4);

  // Frequency hopping flag - 1 bit.
  expected.push_back(config.frequency_hopping_flag & 1U);

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit, reserved.
  std::fill_n(std::back_inserter(expected), 1, 0);

  // Redundancy version - 2 bit.
  expected_pack(expected, config.redundancy_version, 2);

  // HARQ process number - 4 bit, reserved.
  std::fill_n(std::back_inserter(expected), 4, 0);

  // TPC command for scheduled PUCCH - 2 bit.
  expected_pack(expected, config.tpc_command, 2);

  // Number of packed bits so far.
  units::bits nof_packed_bits(expected.size());

  units::bits padding = config.payload_size.padding_incl_ul_sul;

  if (padding > units::bits(0)) {
    // Apply padding, including the UL/SUL reserved field.
    std::fill_n(std::back_inserter(expected), padding.value(), 0);
  }

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 0_0 expected payload scrambled by C-RNTI.
static dci_payload build_dci_0_0_c_rnti_expected(const dci_0_0_c_rnti_configuration& config)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 0, indicating an UL DCI format.
  expected.push_back(0x00U);

  unsigned N_ul_hop = 0;
  if (config.frequency_hopping_flag) {
    N_ul_hop = config.N_ul_hop;
    expected_pack(expected, config.hopping_offset, N_ul_hop);
  }

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = config.payload_size.frequency_resource.value() - N_ul_hop;
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  expected_pack(expected, config.time_resource, 4);

  // Frequency hopping flag - 1 bit.
  expected.push_back(config.frequency_hopping_flag & 1U);

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  expected.push_back(config.new_data_indicator & 1U);

  // Redundancy version - 2 bit.
  expected_pack(expected, config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  expected_pack(expected, config.harq_process_number, 4);

  // TPC command for scheduled PUCCH - 2 bit.
  expected_pack(expected, config.tpc_command, 2);

  units::bits padding_incl_ul_sul = config.payload_size.padding_incl_ul_sul;

  if (padding_incl_ul_sul > units::bits(0)) {
    if (config.ul_sul_indicator.has_value()) {
      // UL/SUL field is present. First, apply padding.
      std::fill_n(std::back_inserter(expected), padding_incl_ul_sul.value() - 1, 0);
      // DCI payload is terminated with the UL/SUL indicator.
      expected.push_back(static_cast<unsigned>(config.ul_sul_indicator.value()) & 1U);
    } else {
      // UL/SUL field is not present, apply padding only.
      std::fill_n(std::back_inserter(expected), padding_incl_ul_sul.value(), 0);
    }
  }

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_0 expected payload scrambled by C-RNTI.
static dci_payload build_dci_1_0_c_rnti_expected(const dci_1_0_c_rnti_configuration& config)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 1, indicating a DL DCI format.
  expected.push_back(0x01U);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = config.payload_size.frequency_resource.value();
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  expected_pack(expected, config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  expected.push_back(config.vrb_to_prb_mapping & 1U);

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  expected.push_back(config.new_data_indicator & 1U);

  // Redundancy version - 2 bit.
  expected_pack(expected, config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  expected_pack(expected, config.harq_process_number, 4);

  // Downlink assignment index - 2 bit.
  expected_pack(expected, config.dl_assignment_index, 2);

  // TPC command for scheduled PUCCH - 2 bit.
  expected_pack(expected, config.tpc_command, 2);

  // PUCCH resource indicator - 3 bit.
  expected_pack(expected, config.pucch_resource_indicator, 3);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  expected_pack(expected, config.pdsch_harq_fb_timing_indicator, 3);

  // Determine the amount of padding, since DCI Format 1_0 scrambled by C-RNTI, CS-RNTI or CS-RNTI can be used in a UE
  // Specific search space.
  unsigned nof_packed_bits  = expected.size();
  unsigned nof_padding_bits = config.payload_size.total.value() - nof_packed_bits;

  // Padding bits.
  std::fill_n(std::back_inserter(expected), nof_padding_bits, 0);

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_0 expected payload scrambled by P-RNTI.
static dci_payload build_dci_1_0_p_rnti_expected(const dci_1_0_p_rnti_configuration& config)
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
    expected_pack(expected, config.short_messages, 8);
  }

  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  if (config.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::short_messages) {
    // If only the short message is carried, the scheduling information for paging bit fields are reserved.
    std::fill_n(std::back_inserter(expected), frequency_resource_nof_bits + 12, 0);
  } else {
    // Frequency domain resource assignment - frequency_resource_nof_bits bits.
    expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

    // Time domain resource assignment - 4 bits.
    expected_pack(expected, config.time_resource, 4);

    // VRB-to-PRB mapping - 1 bit.
    expected.push_back(config.vrb_to_prb_mapping & 1U);

    // Modulation and coding scheme - 5 bits.
    expected_pack(expected, config.modulation_coding_scheme, 5);

    // Transport Block scaling - 2 bits.
    expected_pack(expected, config.tb_scaling, 2);
  }

  // Reserved bits: 6 bits.
  std::fill_n(std::back_inserter(expected), 6, 0);

  // No padding, since DCI Format 1_0 scrambled by P-RNTI is used in common search space.

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_0 expected payload scrambled by SI-RNTI.
static dci_payload build_dci_1_0_si_rnti_expected(const dci_1_0_si_rnti_configuration& config)
{
  // Generate expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  expected_pack(expected, config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  expected.push_back(config.vrb_to_prb_mapping & 1U);

  // Modulation coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // Redundancy version - 2 bits.
  expected_pack(expected, config.redundancy_version, 2);

  // System information indicator - 1 bit.
  expected.push_back(config.system_information_indicator & 1U);

  // Reserved bits - 15 bit.
  std::fill_n(std::back_inserter(expected), 15, 0);

  // No padding, since DCI Format 1_0 scrambled by SI-RNTI is used in common search space.

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_0 expected payload scrambled by RA-RNTI.
static dci_payload build_dci_1_0_ra_rnti_expected(const dci_1_0_ra_rnti_configuration& config)
{
  // Generate expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bits.
  expected_pack(expected, config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  expected.push_back(config.vrb_to_prb_mapping & 1U);

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // Transport Block scaling - 2 bits.
  expected_pack(expected, config.tb_scaling, 2);

  // Reserved bits - 16 bits.
  std::fill_n(std::back_inserter(expected), 16, 0);

  // No padding, since DCI Format 1_0 scrambled by RA-RNTI is used in common search space.

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_0 expected payload scrambled by TC-RNTI.
static dci_payload build_dci_1_0_tc_rnti_expected(const dci_1_0_tc_rnti_configuration& config)
{
  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 1, indicating a DL DCI format.
  expected.push_back(0x01U);

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  unsigned frequency_resource_nof_bits = log2_ceil(config.N_rb_dl_bwp * (config.N_rb_dl_bwp + 1) / 2);
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 4 bit.
  expected_pack(expected, config.time_resource, 4);

  // VRB-to-PRB mapping - 1 bit.
  expected.push_back(config.vrb_to_prb_mapping & 1U);

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // New data indicator - 1 bit.
  expected.push_back(config.new_data_indicator & 1U);

  // Redundancy version - 2 bit.
  expected_pack(expected, config.redundancy_version, 2);

  // HARQ process number - 4 bit.
  expected_pack(expected, config.harq_process_number, 4);

  // Downlink assignment index - 2 bit, reserved.
  std::fill_n(std::back_inserter(expected), 2, 0);

  // TPC command for scheduled PUCCH - 2 bit.
  expected_pack(expected, config.tpc_command, 2);

  // PUCCH resource indicator - 3 bit.
  expected_pack(expected, config.pucch_resource_indicator, 3);

  // PDSCH to HARQ feedback timing indicator - 3 bit.
  expected_pack(expected, config.pdsch_harq_fb_timing_indicator, 3);

  // No padding, since DCI Format 1_0 scrambled by TC-RNTI is used in common search space.

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 0_1 expected payload.
static dci_payload build_dci_0_1_expected(const dci_0_1_configuration& config)
{
  dci_0_1_size payload_size = config.payload_size;

  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 0, indicating an UL DCI format.
  expected.push_back(0x00U);

  if (payload_size.carrier_indicator.value() != 0) {
    // Carrier indicator - 3 bits if present.
    expected_pack(expected, config.carrier_indicator.value(), payload_size.carrier_indicator.value());
  }

  if (payload_size.ul_sul_indicator.value() != 0) {
    // UL/SUL indicator - 1 bit if present.
    expected_pack(
        expected, static_cast<unsigned>(config.ul_sul_indicator.value()), payload_size.ul_sul_indicator.value());
  }

  if (payload_size.bwp_indicator.value() != 0) {
    // BWP indicator - 3 bits if present.
    expected_pack(expected, config.bwp_indicator.value(), payload_size.bwp_indicator.value());
  }

  unsigned frequency_resource_nof_bits = config.payload_size.frequency_resource.value();

  if (config.dynamic_pusch_res_allocation_type.has_value()) {
    // Pack dynamic PUSCH resource allocation flag in the MSB of the frequency domain resource assignment field.
    unsigned dynamic_pusch_alloc =
        config.dynamic_pusch_res_allocation_type.value() == dynamic_resource_allocation::type_0 ? 0 : 1;
    expected.push_back(dynamic_pusch_alloc & 1U);

    // Decrement the number of bits left for frequency domain resource allocation.
    --frequency_resource_nof_bits;
  }

  if (config.frequency_hopping_flag.has_value() && config.frequency_hopping_flag.value() == 1) {
    // Frequency hopping offset - N_ul_hop bits.
    expected_pack(expected, config.hopping_offset.value(), config.N_ul_hop.value());
    frequency_resource_nof_bits -= config.N_ul_hop.value();
  }

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 0, 1, 2, 3 or 4 bits.
  expected_pack(expected, config.time_resource, payload_size.time_resource.value());

  // Frequency hopping flag - 1 bit if present.
  if (payload_size.freq_hopping_flag.value() != 0) {
    expected_pack(expected, config.frequency_hopping_flag.value(), payload_size.freq_hopping_flag.value());
  }

  // Modulation and coding scheme - 5 bits.
  expected_pack(expected, config.modulation_coding_scheme, 5);

  // New data indicator - 1 bits.
  expected.push_back(config.new_data_indicator & 1U);

  // Redundancy version - 2 bits.
  expected_pack(expected, config.redundancy_version, 2);

  // HARQ process number - 4 bits.
  expected_pack(expected, config.harq_process_number, 4);

  // 1st downlink assignment index - 1 or 2 bits.
  expected_pack(expected, config.first_dl_assignment_index, payload_size.first_dl_assignment_idx.value());

  // 2nd downlink assignment index - 2 bits if present.
  if (payload_size.second_dl_assignment_idx.value() != 0) {
    expected_pack(expected, config.second_dl_assignment_index.value(), payload_size.second_dl_assignment_idx.value());
  }

  // TPC command for scheduled PUSCH - 2 bits.
  expected_pack(expected, config.tpc_command, 2);

  // SRS Resource Indicator.
  expected_pack(expected, config.srs_resource_indicator, payload_size.srs_resource_indicator.value());

  // Precoding information and number of layers.
  if (payload_size.precoding_info_nof_layers.value() != 0) {
    expected_pack(expected, config.precoding_info_nof_layers.value(), payload_size.precoding_info_nof_layers.value());
  }

  // Antenna ports for PUSCH transmission - 2, 3, 4 or 5 bits.
  expected_pack(expected, config.antenna_ports, payload_size.antenna_ports.value());

  // SRS request - 2 or 3 bits.
  expected_pack(expected, config.srs_request, payload_size.srs_request.value());

  // CSI request - 0 to 6 bits.
  if (payload_size.csi_request.value() != 0) {
    expected_pack(expected, config.csi_request.value(), payload_size.csi_request.value());
  }

  // CBG Transmission Information (CBGTI) - 0, 2, 4, 6 or 8 bits.
  if (payload_size.cbg_transmission_info.value() != 0) {
    expected_pack(expected, config.cbg_transmission_info.value(), payload_size.cbg_transmission_info.value());
  }

  // PT-RS/DM-RS association - 2 bit if present.
  if (payload_size.ptrs_dmrs_association.value() != 0) {
    expected_pack(expected, config.ptrs_dmrs_association.value(), payload_size.ptrs_dmrs_association.value());
  }

  // Beta offset indicator - 2 bits if present.
  if (payload_size.beta_offset_indicator.value() != 0) {
    expected_pack(expected, config.beta_offset_indicator.value(), payload_size.beta_offset_indicator.value());
  }

  // DM-RS sequence initialization - 1 bit if present.
  if (payload_size.dmrs_seq_initialization.value() != 0) {
    expected_pack(expected, config.dmrs_seq_initialization.value(), payload_size.dmrs_seq_initialization.value());
  }

  // UL-SCH indicator - 1 bit.
  expected.push_back(config.ul_sch_indicator & 1U);

  // Padding bits, if required.
  std::fill_n(std::back_inserter(expected), payload_size.padding.value(), 0);

  return dci_payload(expected.begin(), expected.end());
}

// Builds a DCI Format 1_1 expected payload.
static dci_payload build_dci_1_1_expected(const dci_1_1_configuration& config)
{
  dci_1_1_size payload_size = config.payload_size;

  // Generate the expected payload.
  static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

  // Identifier for DCI formats - 1 bit. This field is always 1, indicating a DL DCI format.
  expected.push_back(0x01U);

  if (payload_size.carrier_indicator.value() != 0) {
    // Carrier indicator - 3 bits if present.
    expected_pack(expected, config.carrier_indicator.value(), payload_size.carrier_indicator.value());
  }

  if (payload_size.bwp_indicator.value() != 0) {
    // BWP indicator - 3 bits if present.
    expected_pack(expected, config.bwp_indicator.value(), payload_size.bwp_indicator.value());
  }

  unsigned frequency_resource_nof_bits = config.payload_size.frequency_resource.value();
  if (config.dynamic_pdsch_res_allocation_type.has_value()) {
    // Pack dynamic PDSCH resource allocation flag in the MSB of the frequency domain resource assignment field.
    unsigned dynamic_pdsch_alloc =
        config.dynamic_pdsch_res_allocation_type.value() == dynamic_resource_allocation::type_0 ? 0 : 1;
    expected.push_back(dynamic_pdsch_alloc & 1U);

    // Decrement the number of bits left for frequency domain resource allocation.
    --frequency_resource_nof_bits;
  }

  // Frequency domain resource assignment - frequency_resource_nof_bits bits.
  expected_pack(expected, config.frequency_resource, frequency_resource_nof_bits);

  // Time domain resource assignment - 0, 1, 2, 3 or 4 bits.
  expected_pack(expected, config.time_resource, payload_size.time_resource.value());

  if (payload_size.vrb_prb_mapping.value() != 0) {
    // VRB-to-PRB mapping - 1 bit if present.
    expected_pack(expected, config.vrb_prb_mapping.value(), payload_size.vrb_prb_mapping.value());
  }

  if (payload_size.prb_bundling_size_indicator.value() != 0) {
    // PRB bundling size indicator - 1 bit if present.
    expected_pack(
        expected, config.prb_bundling_size_indicator.value(), payload_size.prb_bundling_size_indicator.value());
  }

  if (payload_size.rate_matching_indicator.value() != 0) {
    // Rate matching indicator - 0, 1 or 2 bits.
    expected_pack(expected, config.rate_matching_indicator.value(), payload_size.rate_matching_indicator.value());
  }

  if (payload_size.zp_csi_rs_trigger.value() != 0) {
    // ZP CSI-RS trigger - 0, 1 or 2 bits.
    expected_pack(expected, config.zp_csi_rs_trigger.value(), payload_size.zp_csi_rs_trigger.value());
  }

  // Modulation and coding scheme for TB1 - 5 bits.
  expected_pack(expected, config.tb1_modulation_coding_scheme, 5);

  // New data indicator for TB1 - 1 bits.
  expected.push_back(config.tb1_new_data_indicator & 1U);

  // Redundancy version for TB1 - 2 bits.
  expected_pack(expected, config.tb1_redundancy_version, 2);

  if (payload_size.tb2_modulation_coding_scheme.value() != 0) {
    // Modulation and coding scheme for TB2 - 5 bits if present.
    expected_pack(
        expected, config.tb2_modulation_coding_scheme.value(), payload_size.tb2_modulation_coding_scheme.value());
  }

  if (payload_size.tb2_new_data_indicator.value() != 0) {
    // New data indicator for TB 2 - 1 bit if present.
    expected_pack(expected, config.tb2_new_data_indicator.value(), payload_size.tb2_new_data_indicator.value());
  }

  if (payload_size.tb2_redundancy_version.value() != 0) {
    // Redundancy version for TB 2 - 2 bits if present.
    expected_pack(expected, config.tb2_redundancy_version.value(), payload_size.tb2_redundancy_version.value());
  }

  // HARQ process number - 4 bits.
  expected_pack(expected, config.harq_process_number, 4);

  if (payload_size.downlink_assignment_index.value() != 0) {
    // Downlink Assignment Index (DAI) - 0, 2 or 4 bits.
    expected_pack(expected, config.downlink_assignment_index.value(), payload_size.downlink_assignment_index.value());
  }

  // TPC command for scheduled PUSCH - 2 bits.
  expected_pack(expected, config.tpc_command, 2);

  // PUCCH resource indicator - 3 bits.
  expected_pack(expected, config.pucch_resource_indicator, 3);

  if (payload_size.pdsch_harq_fb_timing_indicator.value() != 0) {
    // PDSCH to HARQ feedback timing indicator - 0, 1, 2 or 3 bits.
    expected_pack(
        expected, config.pdsch_harq_fb_timing_indicator.value(), payload_size.pdsch_harq_fb_timing_indicator.value());
  }

  // Antenna ports for PUSCH transmission - 4, 5 or 6 bits.
  expected_pack(expected, config.antenna_ports, payload_size.antenna_ports.value());

  if (payload_size.tx_config_indication.value() != 0) {
    // Transmission configuration indication - 3 bits if present.
    expected_pack(expected, config.tx_config_indication.value(), payload_size.tx_config_indication.value());
  }

  // SRS request - 2 or 3 bits.
  expected_pack(expected, config.srs_request, payload_size.srs_request.value());

  if (payload_size.cbg_transmission_info.value() != 0) {
    // CBG Transmission Information (CBGTI) - 0, 2, 4, 6 or 8 bits.
    expected_pack(expected, config.cbg_transmission_info.value(), payload_size.cbg_transmission_info.value());
  }

  if (payload_size.cbg_flushing_info.value() != 0) {
    // CBG Flushing Information (CBGFI) - 1 bit if present.
    expected_pack(expected, config.cbg_flushing_info.value(), payload_size.cbg_flushing_info.value());
  }

  // DM-RS sequence initialization - 1 bit.
  expected.push_back(config.dmrs_seq_initialization & 1U);

  // Padding bits, if required.
  std::fill_n(std::back_inserter(expected), payload_size.padding.value(), 0);

  return dci_payload(expected.begin(), expected.end());
}

TEST_P(DciFallbackPackingFixture, DciFormatZeroZeroPacking)
{
  for (unsigned i = 0; i != nof_repetitions; ++i) {
    // Generate DCI Format 0_0 scrambled by TC-RNTI configuration.
    dci_0_0_tc_rnti_configuration dci0_0_tc_rnti_cfg =
        build_dci_0_0_tc_rnti_config(aligned_sizes.format0_0_common_size);

    // Generate the DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci0_0_tc_rnti_payload = dci_0_0_tc_rnti_pack(dci0_0_tc_rnti_cfg);

    // Generate expected DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci0_0_tc_rnti_expected = build_dci_0_0_tc_rnti_expected(dci0_0_tc_rnti_cfg);

    // Check DCI payload size.
    ASSERT_EQ(aligned_sizes.format0_0_common_size.total.value(), dci0_0_tc_rnti_payload.size());
    // Assert payload.
    ASSERT_EQ(dci0_0_tc_rnti_expected, dci0_0_tc_rnti_payload);

    // Generate DCI Format 0_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a common
    // search space.
    dci_0_0_c_rnti_configuration dci0_0_c_rnti_cfg_common =
        build_dci_0_0_c_rnti_config(aligned_sizes.format0_0_common_size);

    // Generate DCI Format 0_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
    // UE-specific search space.
    dci_0_0_c_rnti_configuration dci0_0_c_rnti_cfg_ue =
        build_dci_0_0_c_rnti_config(aligned_sizes.format0_0_ue_size.value());

    // Generate the DCI Format 0_0 scrambled by C-RNTI payloads for common and UE-specific search space sets.
    dci_payload dci0_0_c_rnti_payload_common = dci_0_0_c_rnti_pack(dci0_0_c_rnti_cfg_common);
    dci_payload dci0_0_c_rnti_payload_ue     = dci_0_0_c_rnti_pack(dci0_0_c_rnti_cfg_ue);

    // Test DCI Format 0_0 scrambled by C-RNTI packing.
    dci_payload dci0_0_c_rnti_expected_common = build_dci_0_0_c_rnti_expected(dci0_0_c_rnti_cfg_common);
    dci_payload dci0_0_c_rnti_expected_ue     = build_dci_0_0_c_rnti_expected(dci0_0_c_rnti_cfg_ue);

    // Check DCI payload sizes.
    ASSERT_EQ(aligned_sizes.format0_0_common_size.total.value(), dci0_0_c_rnti_payload_common.size());
    ASSERT_EQ(aligned_sizes.format0_0_ue_size.value().total.value(), dci0_0_c_rnti_payload_ue.size());

    // Assert payloads.
    ASSERT_EQ(dci0_0_c_rnti_expected_common, dci0_0_c_rnti_payload_common);
    ASSERT_EQ(dci0_0_c_rnti_expected_ue, dci0_0_c_rnti_payload_ue);
  }
}

TEST_P(DciFallbackPackingFixture, DciFormatOneZeroPacking)
{
  for (unsigned i = 0; i != nof_repetitions; ++i) {
    // BW used to generate the DCI 1_0 configurations. It is set to the CORESET 0 bandwidth if configured, otherwise the
    // initial DL BWP bandwidth.
    unsigned dci1_0_css_bw = (dci_config.coreset0_bw != 0) ? dci_config.coreset0_bw : dci_config.dl_bwp_initial_bw;

    {
      // Generate DCI Format 1_0 scrambled by TC-RNTI configuration.
      dci_1_0_tc_rnti_configuration dci1_0_tc_rnti_cfg = build_dci_1_0_tc_rnti_config(dci1_0_css_bw);

      // Generate the DCI Format 1_0 scrambled by TC-RNTI payload.
      dci_payload dci1_0_tc_rnti_payload = dci_1_0_tc_rnti_pack(dci1_0_tc_rnti_cfg);

      // Test DCI Format 1_0 scrambled by TC-RNTI packing.
      dci_payload dci1_0_tc_rnti_expected = build_dci_1_0_tc_rnti_expected(dci1_0_tc_rnti_cfg);

      // Check DCI payload size.
      ASSERT_EQ(aligned_sizes.format1_0_common_size.total.value(), dci1_0_tc_rnti_payload.size());

      // Assert payload.
      ASSERT_EQ(dci1_0_tc_rnti_expected, dci1_0_tc_rnti_payload);
    }

    {
      // Generate DCI Format 1_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
      // UE-specific search space.
      dci_1_0_c_rnti_configuration dci1_0_c_rnti_cfg_ue =
          build_dci_1_0_c_rnti_config(aligned_sizes.format1_0_ue_size.value());

      // Generate DCI Format 1_0 scrambled by C-RNTI configuration, where the DCI message is monitored in a
      // common search space.
      dci_1_0_c_rnti_configuration dci1_0_c_rnti_cfg_common =
          build_dci_1_0_c_rnti_config(aligned_sizes.format1_0_common_size);

      // Generate the DCI Format 1_0 scrambled by C-RNTI payloads for common and UE-specific search space sets.
      dci_payload dci1_0_c_rnti_payload_common = dci_1_0_c_rnti_pack(dci1_0_c_rnti_cfg_common);
      dci_payload dci1_0_c_rnti_payload_ue     = dci_1_0_c_rnti_pack(dci1_0_c_rnti_cfg_ue);

      // Test DCI Format 1_0 scrambled by C-RNTI packing.
      dci_payload dci1_0_c_rnti_expected_common = build_dci_1_0_c_rnti_expected(dci1_0_c_rnti_cfg_common);
      dci_payload dci1_0_c_rnti_expected_ue     = build_dci_1_0_c_rnti_expected(dci1_0_c_rnti_cfg_ue);

      // Check DCI payload sizes.
      ASSERT_EQ(aligned_sizes.format1_0_common_size.total.value(), dci1_0_c_rnti_payload_common.size());
      ASSERT_EQ(aligned_sizes.format1_0_ue_size.value().total.value(), dci1_0_c_rnti_payload_ue.size());

      // Assert payloads.
      ASSERT_EQ(dci1_0_c_rnti_expected_common, dci1_0_c_rnti_payload_common);
      ASSERT_EQ(dci1_0_c_rnti_expected_ue, dci1_0_c_rnti_payload_ue);
    }

    {
      for (dci_1_0_p_rnti_configuration::payload_info short_messages_indicator :
           {dci_1_0_p_rnti_configuration::payload_info::scheduling_information,
            dci_1_0_p_rnti_configuration::payload_info::short_messages,
            dci_1_0_p_rnti_configuration::payload_info::both}) {
        // Generate the DCI Format 1_0 scrambled by P-RNTI configuration.
        dci_1_0_p_rnti_configuration dci1_0_p_rnti_cfg =
            build_dci_1_0_p_rnti_config(dci1_0_css_bw, short_messages_indicator);

        // Generate the DCI Format 1_0 scrambled by P-RNTI payload.
        dci_payload dci1_0_p_rnti_payload = dci_1_0_p_rnti_pack(dci1_0_p_rnti_cfg);

        // Test DCI Format 1_0 scrambled by P-RNTI packing.
        dci_payload dci1_0_p_rnti_expected = build_dci_1_0_p_rnti_expected(dci1_0_p_rnti_cfg);

        // Check DCI payload size.
        ASSERT_EQ(aligned_sizes.format1_0_common_size.total.value(), dci1_0_p_rnti_payload.size());

        // Assert payload.
        ASSERT_EQ(dci1_0_p_rnti_expected, dci1_0_p_rnti_payload);
      }
    }

    {
      // Generate the DCI Format 1_0 scrambled by SI-RNTI configuration.
      dci_1_0_si_rnti_configuration dci1_0_si_rnti_cfg = build_dci_1_0_si_rnti_config(dci1_0_css_bw);

      // Generate the DCI Format 1_0 scrambled by SI-RNTI payload.
      dci_payload dci1_0_si_rnti_payload = dci_1_0_si_rnti_pack(dci1_0_si_rnti_cfg);

      // Test DCI Format 1_0 scrambled by SI-RNTI packing.
      dci_payload dci1_0_si_rnti_expected = build_dci_1_0_si_rnti_expected(dci1_0_si_rnti_cfg);

      // Check DCI payload size.
      ASSERT_EQ(aligned_sizes.format1_0_common_size.total.value(), dci1_0_si_rnti_payload.size());

      // Assert payload.
      ASSERT_EQ(dci1_0_si_rnti_expected, dci1_0_si_rnti_payload);
    }

    {
      // Generate the DCI Format 1_0 scrambled by RA-RNTI configuration.
      dci_1_0_ra_rnti_configuration dci1_0_ra_rnti_cfg = build_dci_1_0_ra_rnti_config(dci1_0_css_bw);

      // Generate the DCI Format 1_0 scrambled by RA-RNTI payload.
      dci_payload dci1_0_ra_rnti_payload = dci_1_0_ra_rnti_pack(dci1_0_ra_rnti_cfg);

      // Test DCI Format 1_0 scrambled by RA-RNTI packing.
      dci_payload dci1_0_ra_rnti_expected = build_dci_1_0_ra_rnti_expected(dci1_0_ra_rnti_cfg);

      // Check DCI payload size.
      ASSERT_EQ(aligned_sizes.format1_0_common_size.total.value(), dci1_0_ra_rnti_payload.size());

      // Assert payload.
      ASSERT_EQ(dci1_0_ra_rnti_expected, dci1_0_ra_rnti_payload);
    }
  }
}

TEST_P(DciNonFallbackPackingFixture, DciFormatZeroOnePacking)
{
  for (unsigned i = 0; i != nof_repetitions; ++i) {
    // Randomize DCI size configuration parameters that are not in the test case and generate the aligned DCI sizes.
    dci_sizes aligned_sizes = generate_dci_sizes();

    // Assert that unsupported payload fields are not included in the DCI payload format.
    ASSERT_TRUE(aligned_sizes.format0_1_ue_size.value().precoding_info_nof_layers == units::bits(0));
    ASSERT_TRUE(aligned_sizes.format0_1_ue_size.value().ptrs_dmrs_association == units::bits(0));
    ASSERT_TRUE(aligned_sizes.format0_1_ue_size.value().ul_sul_indicator == units::bits(0));

    // Check that the computed DCI Format 0_0 and 1_0 payload sizes match for Common and UE-specific Search Spaces.
    ASSERT_EQ(aligned_sizes.format0_0_common_size.total, aligned_sizes.format1_0_common_size.total);
    ASSERT_EQ(aligned_sizes.format0_0_ue_size.value().total, aligned_sizes.format1_0_ue_size.value().total);

    // Check that the computed DCI Format 0_1 and 1_1 payload sizes do not match with the DCI Format 0_0 and 1_0 payload
    // sizes for UE-specific Search Spaces.
    ASSERT_NE(aligned_sizes.format0_1_ue_size.value().total, aligned_sizes.format0_0_ue_size.value().total);
    ASSERT_NE(aligned_sizes.format1_1_ue_size.value().total, aligned_sizes.format1_0_ue_size.value().total);

    // Generate DCI Format 0_1 configuration.
    dci_0_1_configuration dci0_1_config = build_dci_0_1_config(aligned_sizes.format0_1_ue_size.value());

    // Generate the DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci0_1_payload = dci_0_1_pack(dci0_1_config);

    // Generate expected DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci0_1_expected = build_dci_0_1_expected(dci0_1_config);

    // Check DCI payload size.
    ASSERT_EQ(aligned_sizes.format0_1_ue_size.value().total.value(), dci0_1_payload.size());
    // Assert payload.
    ASSERT_EQ(dci0_1_expected, dci0_1_payload);
  }
}

TEST_P(DciNonFallbackPackingFixture, DciFormatOneOnePacking)
{
  for (unsigned i = 0; i != nof_repetitions; ++i) {
    // Randomize DCI size configuration parameters and generate the aligned DCI sizes.
    dci_sizes aligned_sizes = generate_dci_sizes();

    // Check that the computed DCI Format 0_0 and 1_0 payload sizes match for Common and UE-specific Search Spaces.
    ASSERT_EQ(aligned_sizes.format0_0_common_size.total, aligned_sizes.format1_0_common_size.total);
    ASSERT_EQ(aligned_sizes.format0_0_ue_size.value().total, aligned_sizes.format1_0_ue_size.value().total);

    // Check that the computed DCI Format 0_1 and 1_1 payload sizes do not match with the DCI Format 0_0 and 1_0 payload
    // sizes for UE-specific Search Spaces.
    ASSERT_NE(aligned_sizes.format0_1_ue_size.value().total, aligned_sizes.format0_0_ue_size.value().total);
    ASSERT_NE(aligned_sizes.format1_1_ue_size.value().total, aligned_sizes.format1_0_ue_size.value().total);

    // Generate DCI Format 0_1 configuration.
    dci_1_1_configuration dci1_1_config = build_dci_1_1_config(aligned_sizes.format1_1_ue_size.value());

    // Generate the DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci1_1_payload = dci_1_1_pack(dci1_1_config);

    // Generate expected DCI Format 0_0 scrambled by TC-RNTI payload.
    dci_payload dci1_1_expected = build_dci_1_1_expected(dci1_1_config);

    // Check DCI payload size.
    ASSERT_EQ(aligned_sizes.format1_1_ue_size.value().total.value(), dci1_1_payload.size());
    // Assert payload.
    ASSERT_EQ(dci1_1_expected, dci1_1_payload);
  }
}

TEST_F(DciRarPackingFixture, DciRarPacking)
{
  for (unsigned i = 0; i != nof_repetitions; ++i) {
    // Test DCI RAR packing.
    dci_rar_configuration config    = {};
    config.frequency_hopping_flag   = DciRarPackingFixture::frequency_hopping_flag_dist(rgen);
    config.frequency_resource       = DciRarPackingFixture::frequency_resource_dist(rgen);
    config.time_resource            = DciRarPackingFixture::time_resource_dist(rgen);
    config.modulation_coding_scheme = DciRarPackingFixture::modulation_coding_scheme_dist(rgen);
    config.tpc                      = DciRarPackingFixture::tpc_dist(rgen);
    config.csi_request              = DciRarPackingFixture::csi_request_dist(rgen);

    dci_payload payload = dci_rar_pack(config);

    // Generate expected payload.
    static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> expected;

    // Frequency hopping flag - 1 bit.
    expected.push_back((config.frequency_hopping_flag >> 0U) & 1U);

    // PUSCH frequency resource allocation - 14 bits.
    expected_pack(expected, config.frequency_resource, 14);

    // Time domain resource assignment - 4 bits.
    expected_pack(expected, config.time_resource, 4);

    // Time domain resource assignment - 4 bits.
    expected_pack(expected, config.modulation_coding_scheme, 4);

    // Transmission power control for PUSCH - 3 bits.
    expected_pack(expected, config.tpc, 3);

    // CSI request - 1 bit.
    expected.push_back(config.csi_request & 1U);

    // Assert expected payload.
    ASSERT_EQ(dci_payload(expected.begin(), expected.end()), payload);
  }
}

// Creates test suite that combines all DCI size parameters required for fallback DCI formats.
INSTANTIATE_TEST_SUITE_P(DciPacking,
                         DciFallbackPackingFixture,
                         ::testing::Combine(
                             // Bandwidth of the initial UL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB),
                             // Bandwidth of the active UL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB),
                             // Bandwidth of the initial DL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB),
                             // Bandwidth of the active DL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB)));

// Creates test suite that combines the most relevant DCI size parameters for non-fallback DCI formats.
INSTANTIATE_TEST_SUITE_P(DciPacking,
                         DciNonFallbackPackingFixture,
                         ::testing::Combine(
                             // Bandwidth of the initial UL/DL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB),
                             // Bandwidth of the active UL/DL BWP in number of Resource Blocks.
                             ::testing::Values(12, 96, MAX_RB),
                             // PDSCH HARQ-ACK codebook type.
                             ::testing::Values(pdsch_harq_ack_codebook::semistatic, pdsch_harq_ack_codebook::dynamic),
                             // Number of UL/DL BWP configured by RRC.
                             ::testing::Values(0, 1, 4),
                             // Number of UL/DL time domain resources.
                             ::testing::Values(1, 7, 16),
                             // Non-codebook based transmission.
                             ::testing::Values(false, true),
                             // UL PT-RS configured.
                             ::testing::Values(false, true),
                             // UL/DL resource allocation type.
                             ::testing::Values(resource_allocation::resource_allocation_type_0,
                                               resource_allocation::resource_allocation_type_1,
                                               resource_allocation::dynamic_switch)));
