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

#include "srsran/fapi/message_builders.h"
#include "srsran/fapi_adaptor/mac/messages/ssb.h"
#include "srsran/fapi_adaptor/phy/messages/pdcch.h"
#include "srsran/fapi_adaptor/phy/messages/pdsch.h"
#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/srsvec/bit.h"
#include <chrono>
#include <random>

using namespace srsran;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

/// Benchmark that measures the performance converting SSB data structures from MAC -> FAPI -> PHY.
static void ssb_conversion_benchmark()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> block_index_dist(0, 3000);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 23);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> case_dist(0, 4);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<>        power_dist(-30.8, 30.5);

  for (unsigned i = 0; i != iterations; ++i) {
    srsran::dl_ssb_pdu mac_pdu;
    mac_pdu.pci                        = pci_dist(gen);
    mac_pdu.pss_to_sss_epre            = static_cast<ssb_pss_to_sss_epre>(binary_dist(gen));
    mac_pdu.ssb_index                  = block_index_dist(gen);
    mac_pdu.subcarrier_offset          = subcarrier_offset_dist(gen);
    mac_pdu.offset_to_pointA           = offset_pointA_dist(gen);
    mac_pdu.ssb_case                   = static_cast<ssb_pattern_case>(case_dist(gen));
    mac_pdu.L_max                      = 4;
    mac_pdu.scs                        = subcarrier_spacing::kHz240;
    mac_pdu.mib_data.dmrs_typeA_pos    = binary_dist(gen) == 0 ? dmrs_typeA_position::pos2 : dmrs_typeA_position::pos3;
    mac_pdu.mib_data.pdcch_config_sib1 = sib1_dist(gen);
    mac_pdu.mib_data.cell_barred       = binary_dist(gen);
    mac_pdu.mib_data.intra_freq_reselection = binary_dist(gen);

    fapi::dl_ssb_pdu     fapi_pdu;
    ssb_processor::pdu_t pdu;

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_ssb_mac_to_fapi(fapi_pdu, mac_pdu);
    convert_ssb_fapi_to_phy(pdu, fapi_pdu, sfn_dist(gen), slot_dist(gen), subcarrier_spacing::kHz15);
    auto end = std::chrono::high_resolution_clock::now();

    // Print how much time it took.
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  std::sort(results.begin(), results.end());

  fmt::print("SSB MAC -> FAPI -> PHY conversion Benchmark\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back());
}

static void pdcch_conversion_benchmark()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_int_distribution<unsigned> duration_symbol_dist(0, 3);
  std::uniform_int_distribution<unsigned> shift_index_dist(0, 275);
  std::uniform_int_distribution<unsigned> n_rnti_dist(0, 65535);
  std::uniform_int_distribution<unsigned> cce_dist(0, 135);
  std::uniform_int_distribution<unsigned> aggregation_dist(0, 4);
  std::uniform_int_distribution<unsigned> nid_dmrs_dist(0, 65535);
  std::uniform_int_distribution<unsigned> nid_data_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<int>      power_dist(-8, 8);
  std::uniform_int_distribution<unsigned> custom_dist(2, 3);

  for (unsigned i = 0; i != iterations; ++i) {
    // :TODO: Begin with the MAC structure when it is defined.
    fapi::dl_tti_request_message         msg;
    fapi::dl_tti_request_message_builder builder(msg);
    // :TODO: when the groups are available, add them.
    builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);

    fapi::dl_pdcch_pdu_builder builder_pdcch = builder.add_pdcch_pdu(10);

    freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
                                        1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

    // Always work with the biggest numerology.
    builder_pdcch.set_bwp_parameters(bwp_size_dist(gen),
                                     bwp_start_dist(gen),
                                     subcarrier_spacing::kHz240,
                                     static_cast<cyclic_prefix::options>(binary_dist(gen)));

    builder_pdcch.set_coreset_parameters(
        start_symbol_index_dist(gen),
        duration_symbol_dist(gen),
        freq_domain,
        static_cast<fapi::cce_to_reg_mapping_type>(binary_dist(gen)),
        custom_dist(gen),
        custom_dist(gen),
        static_cast<fapi::pdcch_coreset_type>(binary_dist(gen)),
        shift_index_dist(gen),
        static_cast<coreset_configuration::precoder_granularity_type>(binary_dist(gen)));

    // Add DCI.
    auto builder_dci = builder_pdcch.add_dl_dci();

    builder_dci.set_basic_parameters(
        to_rnti(0), nid_data_dist(gen), n_rnti_dist(gen), cce_dist(gen), aggregation_dist(gen));
    optional<float> profile_nr;
    profile_nr.emplace(power_dist(gen));
    builder_dci.set_tx_power_info_parameter(profile_nr);
    builder_dci.get_tx_precoding_and_beamforming_pdu_builder().set_basic_parameters(275, 0).add_prg(0, {});

    // Payload.
    dci_payload payload;
    builder_dci.set_payload(payload);

    optional<float> profile_data;
    optional<float> profile_dmrs;
    builder_dci.set_maintenance_v3_dci_parameters(true, profile_dmrs, profile_data);
    builder_dci.set_parameters_v4_dci(nid_dmrs_dist(gen));

    pdcch_processor::pdu_t             proc_pdu;
    auto                               pm_tools = generate_precoding_matrix_tables(1);
    const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_pdcch_fapi_to_phy(proc_pdu, msg.pdus[0].pdcch_pdu, msg.sfn, msg.slot, 0, pm_repo);
    auto end = std::chrono::high_resolution_clock::now();

    // Print how much time it took.
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  std::sort(results.begin(), results.end());

  fmt::print("PDCCH MAC -> FAPI -> PHY conversion Benchmark\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back());
}

/// Benchmark that measures the performance converting PDSCH data structures from MAC -> FAPI -> PHY.
#if 0
static void pdsch_conversion_benchmark()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> nid_pdsch_dist(0, 1023);
  std::uniform_int_distribution<unsigned> nof_layers_dist(1, 8);
  std::uniform_int_distribution<unsigned> dmrs_scrambling_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> dmrs_cdm_grps_no_data_dist(1, 3);
  std::uniform_int_distribution<unsigned> nr_of_symbols_dist(1, 14);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_real_distribution<float>   power_dist(-32, 32.0);

  unsigned                       pdu_bitmap          = 0;
  cyclic_prefix_type             cyclic_p            = cyclic_prefix_type::normal;
  pdsch_ref_point_type           ref_point           = pdsch_ref_point_type::point_a;
  dmrs_config_type               config_type         = dmrs_config_type::type_1;
  low_papr_dmrs_type       low_papr            = low_papr_dmrs_type::independent_cdm_group;
  resource_allocation_type resource_alloc      = resource_allocation_type::type_0;
  vrb_to_prb_mapping_type  vrb_prb_mapping     = vrb_to_prb_mapping_type::non_interleaved;
  pdsch_ss_profile_nr_type       power_ss_profile_nr = pdsch_ss_profile_nr_type::dB_minus_3;
  int                            power_nr            = -7;
  int                            power               = -30;
  pdsch_trans_type               trasn_type          = pdsch_trans_type::non_interleaved_other;
  ldpc_base_graph_type           ldpc_graph          = ldpc_base_graph_type::bg_1;
  for (unsigned i = 0; i != iterations; ++i) {
    unsigned sfn                      = sfn_dist(gen);
    unsigned slot                     = slot_dist(gen);
    rnti_t   rnti                     = to_rnti(rnti_dist(gen));
    unsigned bwp_size                 = bwp_size_dist(gen);
    unsigned bwp_start                = bwp_start_dist(gen);
    unsigned nid_pdsch                = nid_pdsch_dist(gen);
    unsigned scrambling_id            = dmrs_scrambling_dist(gen);
    unsigned scrambling_id_complement = dmrs_scrambling_dist(gen);
    bool     n_scid                   = binary_dist(gen);
    unsigned dmrs_cdm_grps_no_data    = dmrs_cdm_grps_no_data_dist(gen);
    unsigned start_symbol_index       = start_symbol_index_dist(gen);
    unsigned nr_of_symbols            = nr_of_symbols_dist(gen);
    unsigned coreset_start            = bwp_size_dist(gen);
    unsigned initial_bwp_size         = bwp_size_dist(gen);
    unsigned tb_size_lbrm_bytes       = bwp_size_dist(gen);
    unsigned dl_dmrs_symbol           = rnti_dist(gen);
    unsigned rb_size                  = nr_of_symbols_dist(gen);
    unsigned rb_start                 = start_symbol_index_dist(gen);
    float    profile_sss              = power_dist(gen);

    std::array<uint8_t, 36> rb_bitmap = {};

    // :TODO: Begin with the MAC structure when it is defined.
    dl_tti_request_message         msg;
    dl_tti_request_message_builder builder(msg);
    // :TODO: when the groups are available, add them.
    builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);
    auto builder_pdsch = builder.add_pdsch_pdu(pdu_bitmap & 1, (pdu_bitmap >> 1) & 1, rnti);

    // Always work with the biggest numerology.
    builder_pdsch.set_bwp_parameters(bwp_size, bwp_start, subcarrier_spacing::kHz240, cyclic_p);
    builder_pdsch.set_codeword_information_parameters(nid_pdsch, 0, 0, ref_point);
    builder_pdsch.set_dmrs_parameters(dl_dmrs_symbol,
                                      config_type,
                                      scrambling_id,
                                      scrambling_id_complement,
                                      low_papr,
                                      n_scid,
                                      dmrs_cdm_grps_no_data,
                                      0);

    auto builder_cw = builder_pdsch.add_codeword();

    unsigned target_code = 2;
    unsigned qam_mod     = 2;
    unsigned mcs         = 20;
    unsigned mcs_table   = 1;
    unsigned rv_index    = 0;
    unsigned tb_size     = 42;

    builder_cw.set_basic_parameters(target_code, qam_mod, mcs, mcs_table, rv_index, tb_size);

    if (resource_alloc == resource_allocation_type::type_0) {
      builder_pdsch.set_pdsch_allocation_in_frequency_type_0({rb_bitmap}, vrb_prb_mapping);
    } else {
      builder_pdsch.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_prb_mapping);
    }

    builder_pdsch.set_pdsch_allocation_in_time_parameters(start_symbol_index, nr_of_symbols);

    optional<int>   profile_nr;
    optional<float> dmrs_profile;
    if (power_nr != -9) {
      profile_nr.emplace(power_nr);
    } else {
      dmrs_profile.emplace(profile_sss);
    }

    builder_pdsch.set_tx_power_info_parameters(profile_nr, power_ss_profile_nr);

    // :TODO: not filling CBG to retx control parameters.

    builder_pdsch.set_maintenance_v3_bwp_parameters(trasn_type, coreset_start, initial_bwp_size);
    builder_pdsch.set_maintenance_v3_codeword_parameters(
        ldpc_graph, tb_size_lbrm_bytes, pdu_bitmap & 1U, (pdu_bitmap >> 1) & 1U);

    optional<float> data_profile;
    if (power != -33) {
      data_profile.emplace(power);
    }

    builder_pdsch.set_maintenance_v3_tx_power_info_parameters(dmrs_profile, data_profile);

    pdsch_processor::pdu_t proc_pdu;

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_pdsch_fapi_to_phy(proc_pdu, msg.pdus[0].pdsch_pdu, sfn, slot);
    auto end = std::chrono::high_resolution_clock::now();

    // Print how much time it took.
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  std::sort(results.begin(), results.end());

  fmt::print("PDSCH MAC -> FAPI -> PHY conversion Benchmark\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back());
}
#endif // 0

int main()
{
  ssb_conversion_benchmark();
  pdcch_conversion_benchmark();
  //  pdsch_conversion_benchmark();

  fmt::print("Success\n");
  return 0;
}
