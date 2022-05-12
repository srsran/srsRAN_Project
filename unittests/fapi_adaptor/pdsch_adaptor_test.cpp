/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi_adaptor/phy/pdsch.h"
#include "srsgnb/support/test_utils.h"
#include <random>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static float ss_profile_nr_to_float(pdsch_ss_profile_nr_type type)
{
  switch (type) {
    case pdsch_ss_profile_nr_type::dB_minus_3:
      return -3.F;
    case pdsch_ss_profile_nr_type::dB0:
      return 0.F;
    case pdsch_ss_profile_nr_type::dB3:
      return 3.F;
    case pdsch_ss_profile_nr_type::dB6:
      return 6.F;
    default:
      return 12.F;
  }
}

/// Returns a PDSCH PDU modulation scheme from the FAPI parameter.
static modulation_scheme get_modulation(uint8_t modulation)
{
  switch (modulation) {
    case 2:
      return modulation_scheme::QPSK;
    case 4:
      return modulation_scheme::QAM16;
    case 6:
      return modulation_scheme::QAM64;
    case 8:
      return modulation_scheme::QAM256;
    default:
      srsran_assert(0, "PDSCH - Invalid modulation order");
      break;
  }

  return modulation_scheme::BPSK;
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

  for (unsigned pdu_bitmap = 0; pdu_bitmap != 4; ++pdu_bitmap) {
    for (auto cyclic_p : {cyclic_prefix_type::normal, cyclic_prefix_type::extended}) {
      for (auto ref_point : {pdsch_ref_point_type::point_a, pdsch_ref_point_type::subcarrier_0}) {
        for (auto config_type : {dmrs_config_type::type_1, dmrs_config_type::type_2}) {
          for (auto low_papr :
               {pdsch_low_papr_dmrs_type::independent_cdm_group, pdsch_low_papr_dmrs_type::dependent_cdm_group}) {
            for (auto resource_alloc :
                 {pdsch_resource_allocation_type::type_0, pdsch_resource_allocation_type::type_1}) {
              for (auto vrb_prb_mapping : {pdsch_vrb_to_prb_mapping_type::non_interleaved,
                                           pdsch_vrb_to_prb_mapping_type::interleaved_rb_size4,
                                           pdsch_vrb_to_prb_mapping_type::interleaved_rb_size2}) {
                for (auto power_ss_profile_nr : {pdsch_ss_profile_nr_type::dB_minus_3,
                                                 pdsch_ss_profile_nr_type::dB0,
                                                 pdsch_ss_profile_nr_type::dB3,
                                                 pdsch_ss_profile_nr_type::dB6,
                                                 pdsch_ss_profile_nr_type::dB6}) {
                  for (int power_nr = -9; power_nr != -7; ++power_nr) {
                    for (int power = -33; power != -30; ++power) {
                      for (auto trasn_type : {pdsch_trans_type::non_interleaved_other,
                                              pdsch_trans_type::non_interleaved_common_ss,
                                              pdsch_trans_type::interleaved_other,
                                              pdsch_trans_type::interleaved_common_type0_coreset0,
                                              pdsch_trans_type::interleaved_common_any_coreset0_present,
                                              pdsch_trans_type::interleaved_common_any_coreset0_not_present}) {
                        for (auto ldpc_graph : {ldpc_base_graph_type::bg_1, ldpc_base_graph_type::bg_2}) {
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

                          std::array<uint8_t, 36> rb_bitmap;

                          dl_pdsch_pdu         pdu;
                          dl_pdsch_pdu_builder builder(pdu);

                          builder.set_basic_parameters(pdu_bitmap & 1, (pdu_bitmap >> 1) & 1, rnti);

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

                          unsigned target_code = 2;
                          unsigned qam_mod     = 2;
                          unsigned mcs         = 20;
                          unsigned mcs_table   = 1;
                          unsigned rv_index    = 0;
                          unsigned tb_size     = 42;

                          builder_cw.set_basic_parameters(target_code, qam_mod, mcs, mcs_table, rv_index, tb_size);

                          if (resource_alloc == pdsch_resource_allocation_type::type_0) {
                            builder.set_pdsch_allocation_in_frequency_type_0({rb_bitmap}, vrb_prb_mapping);
                          } else {
                            builder.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_prb_mapping);
                          }

                          builder.set_pdsch_allocation_in_time_parameters(start_symbol_index, nr_of_symbols);

                          optional<int>   profile_nr;
                          optional<float> dmrs_profile;
                          if (power_nr != -9) {
                            profile_nr.emplace(power_nr);
                          } else {
                            dmrs_profile.emplace(profile_sss);
                          }

                          builder.set_tx_power_info_parameters(profile_nr, power_ss_profile_nr);

                          // :TODO: not filling CBG to retx control parameters.

                          builder.set_maintenance_v3_bwp_parameters(trasn_type, coreset_start, initial_bwp_size);
                          builder.set_maintenance_v3_codeword_parameters(
                              ldpc_graph, tb_size_lbrm_bytes, pdu_bitmap & 1U, (pdu_bitmap >> 1) & 1U);

                          optional<float> data_profile;
                          if (power != -33) {
                            data_profile.emplace(power);
                          }

                          builder.set_maintenance_v3_tx_power_info_parameters(dmrs_profile, data_profile);

                          pdsch_processor::pdu_t proc_pdu;
                          convert_pdsch_fapi_to_phy(proc_pdu, pdu, sfn, slot);

                          // Test basic parameters.
                          TESTASSERT_EQ(sfn, proc_pdu.slot.sfn());
                          TESTASSERT_EQ(slot, proc_pdu.slot.slot_index());
                          TESTASSERT_EQ(static_cast<unsigned>(cyclic_p), static_cast<unsigned>(proc_pdu.cp.value));

                          TESTASSERT_EQ(rnti, proc_pdu.rnti);
                          TESTASSERT_EQ(bwp_size, proc_pdu.bwp_size_rb);
                          TESTASSERT_EQ(bwp_start, proc_pdu.bwp_start_rb);

                          // Codeword.
                          TESTASSERT_EQ(float(target_code), proc_pdu.codewords[0].target_code_rate);
                          TESTASSERT_EQ(get_modulation(qam_mod), proc_pdu.codewords[0].modulation);
                          TESTASSERT_EQ(mcs, proc_pdu.codewords[0].mcs);
                          TESTASSERT_EQ(mcs_table, static_cast<unsigned>(proc_pdu.codewords[0].mcs_table));
                          TESTASSERT_EQ(rv_index, proc_pdu.codewords[0].rv);

                          TESTASSERT_EQ(nid_pdsch, proc_pdu.n_id);

                          for (unsigned i = 0; i != 14; ++i) {
                            TESTASSERT_EQ(((dl_dmrs_symbol >> i) & 1U) == 1U, proc_pdu.dmrs_symbol_mask[i]);
                          }

                          TESTASSERT_EQ(static_cast<unsigned>(ref_point), static_cast<unsigned>(proc_pdu.ref_point));
                          TESTASSERT(dmrs_type((config_type == dmrs_config_type::type_1)
                                                   ? dmrs_type::options::TYPE1
                                                   : dmrs_type::options::TYPE2) == proc_pdu.dmrs);
                          TESTASSERT_EQ(scrambling_id, proc_pdu.scrambling_id);
                          TESTASSERT_EQ(n_scid, proc_pdu.n_scid);
                          TESTASSERT_EQ(dmrs_cdm_grps_no_data, proc_pdu.nof_cdm_groups_without_data);
                          TESTASSERT_EQ(start_symbol_index, proc_pdu.start_symbol_index);
                          TESTASSERT_EQ(nr_of_symbols, proc_pdu.nof_symbols);

                          // :TODO: rb_allocation

                          // Powers.
                          float value = profile_nr ? profile_nr.value() : dmrs_profile.value();
                          TESTASSERT(std::fabs(value - proc_pdu.ratio_pdsch_to_ssb_dB) < 0.001F);

                          TESTASSERT(std::fabs(value - proc_pdu.ratio_pdsch_dmrs_to_sss_dB) < 0.001F);
                          TESTASSERT(std::fabs((data_profile ? data_profile.value() : value) -
                                               proc_pdu.ratio_pdsch_data_to_sss_dB) < 0.001F);

                          TESTASSERT(std::fabs(((power_ss_profile_nr == pdsch_ss_profile_nr_type::L1_use_profile_sss)
                                                    ? value
                                                    : ss_profile_nr_to_float(power_ss_profile_nr)) -
                                               proc_pdu.ratio_nzp_csi_rs_to_ssb_dB) < 0.001F);

                          TESTASSERT_EQ(static_cast<unsigned>(trasn_type),
                                        static_cast<unsigned>(proc_pdu.transmission_type));

                          TESTASSERT_EQ(initial_bwp_size, proc_pdu.initial_dl_bwp_size);
                          TESTASSERT_EQ(coreset_start, proc_pdu.coreset_start_rb);
                          TESTASSERT_EQ((ldpc_graph == ldpc_base_graph_type::bg_1) ? ldpc::base_graph_t::BG1
                                                                                   : ldpc::base_graph_t::BG2,
                                        proc_pdu.ldpc_base_graph);
                          TESTASSERT_EQ(tb_size_lbrm_bytes, proc_pdu.tbs_lbrm_bytes);

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
}

int main()
{
  pdsch_conversion_test();
  fmt::print("PDCCH FAPI to PHY message conversion OK\n");
}