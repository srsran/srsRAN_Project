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
#include "srsran/fapi_adaptor/phy/messages/pdsch.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/support/test_utils.h"
#include <random>

using namespace srsran;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static float calculate_ratio_pdsch_data_to_sss_dB(int                          profile_nr,
                                                  fapi::nzp_csi_rs_epre_to_ssb profile_ss_nr,
                                                  float                        power_data_profile_sss)
{
  if (profile_ss_nr != fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss) {
    float power_control_offset_ss_dB = 0.0F;
    switch (profile_ss_nr) {
      case fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3:
        power_control_offset_ss_dB = -3.F;
        break;
      case fapi::nzp_csi_rs_epre_to_ssb::dB0:
        power_control_offset_ss_dB = .0F;
        break;
      case fapi::nzp_csi_rs_epre_to_ssb::dB3:
        power_control_offset_ss_dB = 3.F;
        break;
      case fapi::nzp_csi_rs_epre_to_ssb::dB6:
      default:
        power_control_offset_ss_dB = 6.F;
        break;
    }

    return static_cast<float>(profile_nr) + power_control_offset_ss_dB;
  }

  return power_data_profile_sss;
}

static float calculate_ratio_pdsch_dmrs_to_sss_dB(int      dmrs_power_profile_sss,
                                                  float    ratio_pdsch_data_to_sss_dB,
                                                  unsigned num_dmrs_cdm_grps_no_data)
{
  if (dmrs_power_profile_sss == -33) {
    static const std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};

    return ratio_pdsch_data_to_sss_dB + beta_dmrs_values[num_dmrs_cdm_grps_no_data];
  }

  return static_cast<float>(dmrs_power_profile_sss);
}

static rb_allocation make_freq_allocation(fapi::pdsch_trans_type         trasn_type,
                                          unsigned                       bwp_start,
                                          unsigned                       bwp_size,
                                          unsigned                       coreset_start,
                                          unsigned                       initial_bwp_size,
                                          fapi::vrb_to_prb_mapping_type  vrb_prb_mapping,
                                          fapi::resource_allocation_type resource_alloc,
                                          std::array<uint8_t, 36>        rb_bitmap,
                                          unsigned                       rb_start,
                                          unsigned                       rb_size)
{
  // Make VRB-to-PRB mapping.
  vrb_to_prb_mapper mapper;
  switch (trasn_type) {
    case fapi::pdsch_trans_type::non_interleaved_common_ss:
      mapper = vrb_to_prb_mapper::create_non_interleaved_common_ss(coreset_start - bwp_start);
      break;
    case fapi::pdsch_trans_type::non_interleaved_other:
      mapper = vrb_to_prb_mapper::create_non_interleaved_other();
      break;
    case fapi::pdsch_trans_type::interleaved_common_type0_coreset0:
      mapper = vrb_to_prb_mapper::create_interleaved_coreset0(coreset_start - bwp_start, initial_bwp_size);
      break;
    case fapi::pdsch_trans_type::interleaved_common_any_coreset0_present:
      mapper = vrb_to_prb_mapper::create_interleaved_common(coreset_start - bwp_start, bwp_start, initial_bwp_size);
      break;
    case fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      mapper = vrb_to_prb_mapper::create_interleaved_common(coreset_start - bwp_start, bwp_start, bwp_size);
      break;
    case fapi::pdsch_trans_type::interleaved_other:
      mapper = vrb_to_prb_mapper::create_interleaved_other(
          bwp_start, bwp_size, vrb_prb_mapping == fapi::vrb_to_prb_mapping_type::interleaved_rb_size2 ? 2 : 4);
      break;
  }

  rb_allocation result;
  if (resource_alloc == fapi::resource_allocation_type::type_0) {
    // Unpack the VRB bitmap. LSB of byte 0 of the bitmap represents the VRB 0.
    bounded_bitset<MAX_RB> vrb_bitmap(bwp_size);
    for (unsigned vrb_index = 0, vrb_index_end = bwp_size; vrb_index != vrb_index_end; ++vrb_index) {
      unsigned byte = vrb_index / 8;
      unsigned bit  = vrb_index % 8;
      if (((rb_bitmap[byte] >> bit) & 1U) == 1U) {
        vrb_bitmap.set(vrb_index);
      }
    }

    result = rb_allocation::make_type0(vrb_bitmap, mapper);
  } else {
    result = rb_allocation::make_type1(rb_start, rb_size, mapper);
  }

  return result;
}

static void pdsch_conversion_test()
{
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

  auto                               pm_tools = generate_precoding_matrix_tables(1);
  const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

  for (auto cyclic_p : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
    for (auto ref_point : {fapi::pdsch_ref_point_type::point_a, fapi::pdsch_ref_point_type::subcarrier_0}) {
      for (auto config_type : {dmrs_config_type::type1, dmrs_config_type::type2}) {
        for (auto low_papr :
             {fapi::low_papr_dmrs_type::independent_cdm_group, fapi::low_papr_dmrs_type::dependent_cdm_group}) {
          for (auto resource_alloc : {fapi::resource_allocation_type::type_0, fapi::resource_allocation_type::type_1}) {
            // Iterate possible VRB-to PRB mapping. As transmission type is enabled
            // vrb_to_prb_mapping_type::non_interleaved value is irrelevant.
            for (auto vrb_prb_mapping : {fapi::vrb_to_prb_mapping_type::interleaved_rb_size4,
                                         fapi::vrb_to_prb_mapping_type::interleaved_rb_size2}) {
              // Iterate all possible NZP-CSI-RS to SSS ratios. L1_use_profile_sss means SSS profile mode.
              for (auto power_ss_profile_nr : {fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3,
                                               fapi::nzp_csi_rs_epre_to_ssb::dB0,
                                               fapi::nzp_csi_rs_epre_to_ssb::dB3,
                                               fapi::nzp_csi_rs_epre_to_ssb::dB6,
                                               fapi::nzp_csi_rs_epre_to_ssb::dB6,
                                               fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss}) {
                // Iterate possible PDSCH data to NZP-CSI-RS ratios for Profile NR. It is ignored when
                // power_ss_profile_nr is L1_use_profile_sss.
                for (int power_profile_nr = -8; power_profile_nr != -7; ++power_profile_nr) {
                  // Iterate possible PDSCH DMRS to SSS ratios. -33 for Profile NR.
                  for (int dmrs_power_profile_sss = -33; dmrs_power_profile_sss != -30; ++dmrs_power_profile_sss) {
                    for (auto trasn_type : {fapi::pdsch_trans_type::non_interleaved_other,
                                            fapi::pdsch_trans_type::non_interleaved_common_ss,
                                            fapi::pdsch_trans_type::interleaved_other,
                                            fapi::pdsch_trans_type::interleaved_common_type0_coreset0,
                                            fapi::pdsch_trans_type::interleaved_common_any_coreset0_present,
                                            fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present}) {
                      for (auto ldpc_graph : {ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2}) {
                        unsigned     sfn                      = sfn_dist(gen);
                        unsigned     slot                     = slot_dist(gen);
                        rnti_t       rnti                     = to_rnti(rnti_dist(gen));
                        unsigned     bwp_size                 = bwp_size_dist(gen);
                        unsigned     bwp_start                = bwp_start_dist(gen);
                        unsigned     nid_pdsch                = nid_pdsch_dist(gen);
                        unsigned     scrambling_id            = dmrs_scrambling_dist(gen);
                        unsigned     scrambling_id_complement = dmrs_scrambling_dist(gen);
                        bool         n_scid                   = binary_dist(gen);
                        unsigned     dmrs_cdm_grps_no_data    = dmrs_cdm_grps_no_data_dist(gen);
                        unsigned     start_symbol_index       = start_symbol_index_dist(gen);
                        unsigned     nr_of_symbols            = nr_of_symbols_dist(gen);
                        unsigned     coreset_start            = bwp_size_dist(gen);
                        unsigned     initial_bwp_size         = bwp_size_dist(gen);
                        units::bytes tb_size_lbrm_bytes{50};
                        unsigned     dl_dmrs_symbol         = rnti_dist(gen);
                        unsigned     rb_size                = nr_of_symbols_dist(gen);
                        unsigned     rb_start               = start_symbol_index_dist(gen);
                        float        power_data_profile_sss = 1000.0F * std::round(power_dist(gen) / 1000.0F);

                        std::array<uint8_t, 36> rb_bitmap = {};

                        fapi::dl_pdsch_pdu         pdu;
                        fapi::dl_pdsch_pdu_builder builder(pdu);

                        builder.set_basic_parameters(rnti);

                        // Always work with the biggest numerology.
                        builder.set_bwp_parameters(bwp_size, bwp_start, subcarrier_spacing::kHz240, cyclic_p);
                        builder.set_codeword_information_parameters(nid_pdsch, 0, 0, ref_point);
                        builder.set_dmrs_parameters(dl_dmrs_symbol,
                                                    config_type,
                                                    scrambling_id,
                                                    scrambling_id_complement,
                                                    low_papr,
                                                    n_scid,
                                                    dmrs_cdm_grps_no_data,
                                                    0);

                        auto builder_cw = builder.add_codeword();

                        unsigned     target_code = 2;
                        unsigned     qam_mod     = 2;
                        unsigned     mcs         = 20;
                        unsigned     mcs_table   = 1;
                        unsigned     rv_index    = 0;
                        units::bytes tb_size{42};

                        builder_cw.set_basic_parameters(target_code, qam_mod, mcs, mcs_table, rv_index, tb_size);

                        if (resource_alloc == fapi::resource_allocation_type::type_0) {
                          builder.set_pdsch_allocation_in_frequency_type_0({rb_bitmap}, vrb_prb_mapping);
                        } else {
                          builder.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_prb_mapping);
                        }

                        builder.set_pdsch_allocation_in_time_parameters(start_symbol_index, nr_of_symbols);

                        optional<int>   profile_nr;
                        optional<float> data_profile_sss;
                        optional<float> dmrs_profile_sss;
                        if (power_ss_profile_nr != fapi::nzp_csi_rs_epre_to_ssb::L1_use_profile_sss) {
                          profile_nr.emplace(power_profile_nr);
                        } else {
                          data_profile_sss.emplace(power_data_profile_sss);
                        }
                        if (dmrs_power_profile_sss != -33) {
                          dmrs_profile_sss.emplace(dmrs_power_profile_sss);
                        }

                        builder.set_tx_power_info_parameters(profile_nr, power_ss_profile_nr);

                        // :TODO: not filling CBG to retx control parameters.

                        builder.set_maintenance_v3_bwp_parameters(trasn_type, coreset_start, initial_bwp_size);
                        builder.set_maintenance_v3_codeword_parameters(ldpc_graph, tb_size_lbrm_bytes, 0, 0);
                        builder.set_maintenance_v3_tx_power_info_parameters(dmrs_profile_sss, data_profile_sss);
                        builder.get_tx_precoding_and_beamforming_pdu_builder().add_prg(0, {}).set_basic_parameters(51,
                                                                                                                   0);

                        pdsch_processor::pdu_t proc_pdu;
                        convert_pdsch_fapi_to_phy(proc_pdu, pdu, sfn, slot, {}, pm_repo);

                        // Test basic parameters.
                        TESTASSERT_EQ(sfn, proc_pdu.slot.sfn());
                        TESTASSERT_EQ(slot, proc_pdu.slot.slot_index());
                        TESTASSERT_EQ(static_cast<unsigned>(cyclic_p), static_cast<unsigned>(proc_pdu.cp.value));

                        TESTASSERT_EQ(rnti, proc_pdu.rnti);
                        TESTASSERT_EQ(bwp_size, proc_pdu.bwp_size_rb);
                        TESTASSERT_EQ(bwp_start, proc_pdu.bwp_start_rb);

                        // Codeword.
                        TESTASSERT_EQ(static_cast<modulation_scheme>(qam_mod), proc_pdu.codewords[0].modulation);
                        TESTASSERT_EQ(rv_index, proc_pdu.codewords[0].rv);

                        TESTASSERT_EQ(nid_pdsch, proc_pdu.n_id);

                        for (unsigned i = 0; i != 14; ++i) {
                          TESTASSERT_EQ(((dl_dmrs_symbol >> i) & 1U) == 1U, proc_pdu.dmrs_symbol_mask.test(i));
                        }

                        TESTASSERT_EQ(static_cast<unsigned>(ref_point), static_cast<unsigned>(proc_pdu.ref_point));
                        TESTASSERT(dmrs_type((config_type == dmrs_config_type::type1)
                                                 ? dmrs_type::options::TYPE1
                                                 : dmrs_type::options::TYPE2) == proc_pdu.dmrs);
                        TESTASSERT_EQ(scrambling_id, proc_pdu.scrambling_id);
                        TESTASSERT_EQ(n_scid, proc_pdu.n_scid);
                        TESTASSERT_EQ(dmrs_cdm_grps_no_data, proc_pdu.nof_cdm_groups_without_data);
                        TESTASSERT_EQ(start_symbol_index, proc_pdu.start_symbol_index);
                        TESTASSERT_EQ(nr_of_symbols, proc_pdu.nof_symbols);

                        // Powers.
                        float ratio_pdsch_data_to_sss_dB = calculate_ratio_pdsch_data_to_sss_dB(
                            power_profile_nr, power_ss_profile_nr, power_data_profile_sss);
                        float ratio_pdsch_dmrs_to_sss_dB = calculate_ratio_pdsch_dmrs_to_sss_dB(
                            dmrs_power_profile_sss, ratio_pdsch_data_to_sss_dB, dmrs_cdm_grps_no_data);

                        TESTASSERT(std::fabs(ratio_pdsch_dmrs_to_sss_dB - proc_pdu.ratio_pdsch_dmrs_to_sss_dB) < 0.001F,
                                   "PDSCH DMRS to SSS power ratio {} is too far from expected {}.",
                                   proc_pdu.ratio_pdsch_dmrs_to_sss_dB,
                                   ratio_pdsch_dmrs_to_sss_dB);
                        TESTASSERT(std::fabs(ratio_pdsch_data_to_sss_dB - proc_pdu.ratio_pdsch_data_to_sss_dB) < 0.001F,
                                   "PDSCH Data to SSS power ratio {} is too far from expected {}.",
                                   proc_pdu.ratio_pdsch_data_to_sss_dB,
                                   ratio_pdsch_data_to_sss_dB);

                        // Frequency domain allocation.
                        rb_allocation freq_allocation = make_freq_allocation(trasn_type,
                                                                             bwp_start,
                                                                             bwp_size,
                                                                             coreset_start,
                                                                             initial_bwp_size,
                                                                             vrb_prb_mapping,
                                                                             resource_alloc,
                                                                             rb_bitmap,
                                                                             rb_start,
                                                                             rb_size);
                        TESTASSERT(freq_allocation == proc_pdu.freq_alloc);

                        TESTASSERT_EQ(ldpc_graph, proc_pdu.ldpc_base_graph);
                        TESTASSERT_EQ(tb_size_lbrm_bytes.value(), proc_pdu.tbs_lbrm_bytes);

                        // :TODO: re_pattern.
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

int main()
{
  pdsch_conversion_test();
  fmt::print("PDCCH FAPI to PHY message conversion OK\n");
}
