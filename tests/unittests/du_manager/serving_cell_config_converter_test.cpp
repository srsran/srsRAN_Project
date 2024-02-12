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

#include "lib/du_manager/converters/asn1_rrc_config_helpers.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

srs_du::cell_group_config make_initial_cell_group_config()
{
  srs_du::cell_group_config dest_cell_grp_cfg{};
  dest_cell_grp_cfg.cells.emplace(0, config_helpers::create_default_initial_ue_spcell_cell_config());
  dest_cell_grp_cfg.mcg_cfg = config_helpers::make_initial_mac_cell_group_config();

  return dest_cell_grp_cfg;
}

pusch_config make_initial_pusch_config()
{
  pusch_config cfg{};
  cfg.tx_cfg = pusch_config::tx_config::codebook;
  cfg.pusch_mapping_type_b_dmrs.emplace();
  cfg.pusch_mapping_type_b_dmrs.value().trans_precoder_disabled.emplace();
  cfg.pusch_mapping_type_b_dmrs.value().ptrs = ptrs_uplink_config{
      .trans_precoder_disabled = {ptrs_uplink_config::transform_precoder_disabled{.f_density = {2, 4},
                                                                                  .t_density = {7, 8}}},
      .trans_precoder_enabled  = {ptrs_uplink_config::transform_precoder_enabled{.sampl_density = {2, 5}}}};

  cfg.pusch_pwr_ctrl = pusch_config::pusch_power_control{.msg3_alpha               = alpha::alpha1,
                                                         .p0_nominal_without_grant = -76,
                                                         .p0_alphasets             = {},
                                                         .pathloss_ref_rs          = {},
                                                         .sri_pusch_mapping        = {}};
  cfg.pusch_pwr_ctrl.value().p0_alphasets.push_back(pusch_config::pusch_power_control::p0_pusch_alphaset{
      .id = static_cast<p0_pusch_alphaset_id>(0), .p0 = 0, .p0_pusch_alpha = alpha::alpha1});
  cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
      .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .rs = static_cast<ssb_id_t>(0)});
  cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
      .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(0),
      .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .sri_p0_pusch_alphaset_id     = static_cast<p0_pusch_alphaset_id>(0),
      .closed_loop_idx = pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0});
  cfg.res_alloc = pusch_config::resource_allocation::resource_allocation_type_1;
  cfg.pusch_td_alloc_list.resize(1);
  cfg.pusch_td_alloc_list[0].k2       = 4;
  cfg.pusch_td_alloc_list[0].map_type = sch_mapping_type::typeA;
  cfg.pusch_td_alloc_list[0].symbols  = {0, 14};
  cfg.trans_precoder                  = pusch_config::transform_precoder::disabled;
  cfg.cb_subset                       = pusch_config::codebook_subset::non_coherent;
  cfg.max_rank                        = 1;

  cfg.uci_cfg.emplace();
  auto& uci_cfg   = cfg.uci_cfg.value();
  uci_cfg.scaling = alpha_scaling_opt::f1;
  beta_offsets b_offset{};
  b_offset.beta_offset_ack_idx_1    = 9;
  b_offset.beta_offset_ack_idx_2    = 9;
  b_offset.beta_offset_ack_idx_3    = 9;
  b_offset.beta_offset_csi_p1_idx_1 = 6;
  b_offset.beta_offset_csi_p1_idx_2 = 6;
  b_offset.beta_offset_csi_p2_idx_1 = 6;
  b_offset.beta_offset_csi_p2_idx_2 = 6;
  uci_cfg.beta_offsets_cfg          = uci_on_pusch::beta_offsets_semi_static{b_offset};

  return cfg;
}

srs_config make_initial_srs_config()
{
  srs_config cfg{};

  cfg.srs_res_set.push_back(srs_config::srs_resource_set{
      .id              = static_cast<srs_config::srs_res_set_id>(0),
      .srs_res_id_list = {static_cast<srs_config::srs_res_id>(0)},
      .res_type =
          srs_config::srs_resource_set::aperiodic_resource_type{.aperiodic_srs_res_trigger = 1, .slot_offset = 2},
      .srs_res_set_usage  = srs_config::srs_resource_set::usage::codebook,
      .srs_pwr_ctrl_alpha = alpha::alpha1,
      .p0                 = -80});
  cfg.srs_res.push_back(srs_config::srs_resource{
      .id                      = static_cast<srs_config::srs_res_id>(0),
      .nof_ports               = srs_config::srs_resource::nof_srs_ports::port1,
      .trans_comb_value        = 2,
      .trans_comb_offset       = 0,
      .trans_comb_cyclic_shift = 0,
      .res_mapping =
          srs_config::srs_resource::resource_mapping{
              .start_pos = 2,
              .nof_symb  = srs_config::srs_resource::resource_mapping::nof_symbols::n1,
              .re_factor = srs_config::srs_resource::resource_mapping::repetition_factor::n1},
      .freq_domain_pos       = 0,
      .freq_domain_shift     = 0,
      .freq_hop              = srs_config::srs_resource::frequency_hopping{.c_srs = 0, .b_srs = 0, .b_hop = 0},
      .grp_or_seq_hop        = srs_config::srs_resource::group_or_sequence_hopping::neither,
      .res_type              = srs_config::srs_resource::aperiodic,
      .sequence_id           = 40,
      .spatial_relation_info = srs_config::srs_resource::srs_spatial_relation_info{
          .reference_signal = static_cast<nzp_csi_rs_res_id_t>(0)}});

  return cfg;
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_pdcch_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present, dest_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.has_value());
  // Since its initial setup and no source cell group config was provided pdcch must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup().coreset_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.value().coresets.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup().coreset_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup().search_spaces_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.value().search_spaces.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup().search_spaces_to_release_list.size(), 0);
  }
}

TEST(serving_cell_config_converter_test, test_ue_pdcch_cfg_release_conversion)
{
  auto                      src_cell_grp_cfg  = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg = make_initial_cell_group_config();
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.reset();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present,
            not dest_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.has_value());
  // PDCCH Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.type(), asn1::setup_release_opts::release);
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_pdsch_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present, dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value());
  // Since its initial setup and no source cell group config was provided pdsch must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().data_scrambling_id_pdsch_present,
              dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().data_scrambling_id_pdsch.has_value());
    if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().data_scrambling_id_pdsch.has_value()) {
      ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().data_scrambling_id_pdsch,
                dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().data_scrambling_id_pdsch.value());
    }

    if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().pdsch_mapping_type_a_dmrs.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a.is_setup());
    }

    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().tci_states_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().tci_states.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().tci_states_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().rate_match_pattern_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().rate_match_pattrn.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().rate_match_pattern_to_release_list.size(), 0);
  }
}

TEST(serving_cell_config_converter_test, test_ue_pdsch_cfg_release_conversion)
{
  srs_du::cell_group_config dest_cell_grp_cfg = make_initial_cell_group_config();
  auto                      src_cell_grp_cfg  = dest_cell_grp_cfg;
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg.reset();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present,
            not dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value());
  // PDSCH Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.type(), asn1::setup_release_opts::release);
}

TEST(serving_cell_config_converter_test, test_ue_custom_pdsch_cfg_conversion)
{
  auto                      src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup.
  auto& dest_pdsch_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg.value();
  dest_pdsch_cfg.tci_states.push_back(tci_state{
      .state_id  = static_cast<tci_state_id_t>(1),
      .qcl_type1 = {.ref_sig  = {.type   = qcl_info::reference_signal::reference_signal_type::csi_rs,
                                 .csi_rs = static_cast<nzp_csi_rs_res_id_t>(0)},
                    .qcl_type = qcl_info::qcl_type::type_a},
  });
  dest_pdsch_cfg.tci_states.erase(dest_pdsch_cfg.tci_states.begin());

  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().additional_positions = dmrs_additional_positions::pos0;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id0       = 10;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id1       = 20;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present, dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value()) {
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a.is_setup());

    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().tci_states_to_add_mod_list.size(), 1);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().tci_states_to_release_list.size(), 1);
  }
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_uplink_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());

  if (dest_sp_cell_cfg_ded.ul_config.has_value()) {
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present,
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.has_value());
    if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.is_setup());

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list.size(),
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_set.size());
      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_release_list.size(), 0);

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.size(),
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list.size());
      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_release_list.size(), 0);

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format1_present,
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_1_common_param.has_value());
      if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_1_common_param.has_value()) {
        ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format1.is_setup());
      }

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2_present,
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_2_common_param.has_value());
      if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_2_common_param.has_value()) {
        ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2.is_setup());
      }

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3_present,
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_3_common_param.has_value());
      if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_3_common_param.has_value()) {
        ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3.is_setup());
      }

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4_present,
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_4_common_param.has_value());
      if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_4_common_param.has_value()) {
        ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4.is_setup());
      }

      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.size(),
                dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list.size());
      ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_release_list.size(), 0);
    }
  }
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_dl_bwp_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_EQ(rrc_sp_cell_cfg_ded.dl_bwp_to_add_mod_list.size(), dest_sp_cell_cfg_ded.dl_bwps.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.dl_bwp_to_release_list.size(), 0);
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_pucch_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present);
  // Since its initial setup and no source cell group config was provided pucch must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_set.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack.size());

    if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_1_common_param.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format1_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format1.is_setup());
    }

    if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_2_common_param.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2.is_setup());
    }

    if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_3_common_param.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3.is_setup());
    }

    if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().format_4_common_param.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4.is_setup());
    }
  }
}

TEST(serving_cell_config_converter_test, test_ue_custom_pucch_cfg_conversion)
{
  auto                      src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup. Assume PUCCH Config is present in initial cell group config.
  auto& dest_pucch_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

  // >> PUCCH Resource Set 1.
  dest_pucch_cfg.pucch_res_set.emplace_back();
  dest_pucch_cfg.pucch_res_set.back().pucch_res_set_id = 1;
  dest_pucch_cfg.pucch_res_set.back().pucch_res_id_list.emplace_back(pucch_res_id_t{1, 1});
  // Remove first element.
  dest_pucch_cfg.pucch_res_set.erase(dest_pucch_cfg.pucch_res_set.begin());

  // >>> PUCCH resource 2.
  pucch_resource res_basic{
      .res_id = pucch_res_id_t{12, 12}, .starting_prb = 40, .second_hop_prb = 50, .format = pucch_format::FORMAT_3};
  res_basic.format = pucch_format::FORMAT_2;
  res_basic.format_params.emplace<pucch_format_2_3_cfg>(pucch_format_2_3_cfg{.nof_symbols = 1, .starting_sym_idx = 13});
  dest_pucch_cfg.pucch_res_list.push_back(res_basic);

  // Remove first element.
  dest_pucch_cfg.pucch_res_list.erase(dest_pucch_cfg.pucch_res_list.begin());

  dest_pucch_cfg.dl_data_to_ul_ack.push_back(2);
  dest_pucch_cfg.dl_data_to_ul_ack.push_back(3);

  // Release default one.
  dest_pucch_cfg.format_1_common_param.reset();

  // Note: Set a configuration different from the one already existing the starting config; this will trigger the
  // calculate_cell_group_config_diff() to add an element in format2 in the generated ASN1 message.
  dest_pucch_cfg.format_2_common_param.emplace();
  dest_pucch_cfg.format_2_common_param.value().max_c_rate                = max_pucch_code_rate::dot_08;
  dest_pucch_cfg.format_2_common_param.value().simultaneous_harq_ack_csi = true;

  dest_pucch_cfg.format_3_common_param.emplace();
  dest_pucch_cfg.format_3_common_param.value().max_c_rate = max_pucch_code_rate::dot_08;
  dest_pucch_cfg.format_3_common_param.value().nof_slots  = pucch_common_all_formats::num_of_slots::n2;

  dest_pucch_cfg.format_4_common_param.emplace();
  dest_pucch_cfg.format_4_common_param.value().max_c_rate      = max_pucch_code_rate::dot_60;
  dest_pucch_cfg.format_4_common_param.value().additional_dmrs = true;

  // >>> SR Resource.
  dest_pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 2,
                                                                          .sr_id        = uint_to_sched_req_id(1),
                                                                          .period       = sr_periodicity::sl_80,
                                                                          .offset       = 0,
                                                                          .pucch_res_id = pucch_res_id_t{1, 1}});

  // Remove first element.
  dest_pucch_cfg.sr_res_list.erase(dest_pucch_cfg.sr_res_list.begin());

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present);
  // Since its initial setup and no source cell group config was provided pucch must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list.size(), 1);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_set_to_release_list.size(), 1);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.size(), 1);
    auto& pucch_res1 = rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[0];
    ASSERT_EQ(pucch_res1.pucch_res_id, 12);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_release_list.size(), 1);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.size(), 1);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_release_list.size(), 1);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack.size());

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format1.type(),
              asn1::setup_release_opts::release);

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format2.is_setup());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format3.is_setup());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().format4.is_setup());
  }
}

TEST(serving_cell_config_converter_test, test_ue_pucch_cfg_release_conversion)
{
  auto                      src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present,
            not dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pucch_cfg.has_value());
  // PUCCH Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.type(), asn1::setup_release_opts::release);
}

TEST(serving_cell_config_converter_test, test_initial_pusch_cfg_conversion)
{
  auto dest_cell_grp_cfg                                                           = make_initial_cell_group_config();
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg = make_initial_pusch_config();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg_present);
  // Since its initial setup and no source cell group config was provided pusch must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pusch_cfg.has_value()) {
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_b_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_b.is_setup());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup()
                    .dmrs_ul_for_pusch_map_type_b.setup()
                    .phase_tracking_rs_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup()
                    .dmrs_ul_for_pusch_map_type_b.setup()
                    .phase_tracking_rs.is_setup());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl_present);
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.pathloss_ref_rs_to_add_mod_list.size(),
        dest_sp_cell_cfg_ded.ul_config.value()
            .init_ul_bwp.pusch_cfg.value()
            .pusch_pwr_ctrl.value()
            .pathloss_ref_rs.size());
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.pathloss_ref_rs_to_release_list.size(),
        0);

    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.sri_pusch_map_to_add_mod_list.size(),
        dest_sp_cell_cfg_ded.ul_config.value()
            .init_ul_bwp.pusch_cfg.value()
            .pusch_pwr_ctrl.value()
            .sri_pusch_mapping.size());
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.sri_pusch_map_to_release_list.size(),
        0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.p0_alpha_sets.size(),
              dest_sp_cell_cfg_ded.ul_config.value()
                  .init_ul_bwp.pusch_cfg.value()
                  .pusch_pwr_ctrl.value()
                  .p0_alphasets.size());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_time_domain_alloc_list_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_time_domain_alloc_list.is_setup());
  }
}

TEST(serving_cell_config_converter_test, test_ue_custom_pusch_cfg_conversion)
{
  auto src_cell_grp_cfg                                                           = make_initial_cell_group_config();
  src_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg = make_initial_pusch_config();

  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  auto& dest_pusch_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value();
  // Add new/remove configurations.
  dest_pusch_cfg.pusch_mapping_type_a_dmrs.emplace();
  dest_pusch_cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_enabled.emplace();
  dest_pusch_cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_enabled.value().is_seq_hopping_enabled = true;
  // Release.
  dest_pusch_cfg.pusch_mapping_type_b_dmrs.reset();

  dest_pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.push_back(pusch_config::pusch_power_control::p0_pusch_alphaset{
      .id = static_cast<p0_pusch_alphaset_id>(1), .p0 = 2, .p0_pusch_alpha = alpha::alpha05});
  // Erase/Release.
  dest_pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.erase(dest_pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.begin());

  dest_pusch_cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(
      pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
          .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(1),
          .rs = static_cast<nzp_csi_rs_res_id_t>(0)});
  // Erase/Release.
  dest_pusch_cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.erase(
      dest_pusch_cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.begin());

  dest_pusch_cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(
      pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
          .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(1),
          .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(1),
          .sri_p0_pusch_alphaset_id     = static_cast<p0_pusch_alphaset_id>(1),
          .closed_loop_idx = pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0});
  // Erase/Release.
  dest_pusch_cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.erase(
      dest_pusch_cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.begin());

  dest_pusch_cfg.pusch_td_alloc_list.push_back(pusch_time_domain_resource_allocation{
      .k2 = 4, .map_type = srsran::sch_mapping_type::typeB, .symbols = ofdm_symbol_range{2, 12}});

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pusch_cfg.has_value()) {
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_a_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_a.is_setup());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_b_present);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().dmrs_ul_for_pusch_map_type_b.type(),
              asn1::setup_release_opts::release);

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl_present);
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.pathloss_ref_rs_to_add_mod_list.size(),
        1);
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.pathloss_ref_rs_to_release_list.size(),
        1);

    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.sri_pusch_map_to_add_mod_list.size(),
        1);
    ASSERT_EQ(
        rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.sri_pusch_map_to_release_list.size(),
        1);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_pwr_ctrl.p0_alpha_sets.size(),
              dest_sp_cell_cfg_ded.ul_config.value()
                  .init_ul_bwp.pusch_cfg.value()
                  .pusch_pwr_ctrl.value()
                  .p0_alphasets.size());

    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_time_domain_alloc_list_present);
    ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup().pusch_time_domain_alloc_list.is_setup());
  }
}

TEST(serving_cell_config_converter_test, test_ue_pusch_cfg_release_conversion)
{
  auto src_cell_grp_cfg                                                           = make_initial_cell_group_config();
  src_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg = make_initial_pusch_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg_present,
            not dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.pusch_cfg.has_value());
  // PUSCH Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.type(), asn1::setup_release_opts::release);
}

TEST(serving_cell_config_converter_test, test_initial_srs_cfg_conversion)
{
  auto dest_cell_grp_cfg                                                         = make_initial_cell_group_config();
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg = make_initial_srs_config();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg_present);
  // Since its initial setup and no source cell group config was provided srs config must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_set_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.srs_cfg.value().srs_res_set.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_set_to_release_list.size(), 0);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_to_add_mod_list.size(),
              dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.srs_cfg.value().srs_res_set.size());
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_to_release_list.size(), 0);
  }
}

TEST(serving_cell_config_converter_test, test_ue_custom_srs_cfg_conversion)
{
  auto src_cell_grp_cfg                                                         = make_initial_cell_group_config();
  src_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg = make_initial_srs_config();

  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  auto& dest_pusch_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  // Add new/remove configurations.
  dest_pusch_cfg.srs_res_set.push_back(srs_config::srs_resource_set{
      .id                 = static_cast<srs_config::srs_res_set_id>(1),
      .srs_res_id_list    = {static_cast<srs_config::srs_res_id>(1)},
      .res_type           = srs_config::srs_resource_set::semi_persistent_resource_type{.associated_csi_rs =
                                                                                  static_cast<nzp_csi_rs_res_id_t>(1)},
      .srs_res_set_usage  = srs_config::srs_resource_set::usage::non_codebook,
      .srs_pwr_ctrl_alpha = alpha::alpha07,
      .p0                 = -70});
  // Release.
  dest_pusch_cfg.srs_res_set.erase(dest_pusch_cfg.srs_res_set.begin());

  dest_pusch_cfg.srs_res.push_back(srs_config::srs_resource{
      .id                      = static_cast<srs_config::srs_res_id>(1),
      .nof_ports               = srs_config::srs_resource::nof_srs_ports::port1,
      .trans_comb_value        = 4,
      .trans_comb_offset       = 0,
      .trans_comb_cyclic_shift = 0,
      .res_mapping =
          srs_config::srs_resource::resource_mapping{
              .start_pos = 50,
              .nof_symb  = srs_config::srs_resource::resource_mapping::nof_symbols::n1,
              .re_factor = srs_config::srs_resource::resource_mapping::repetition_factor::n1},
      .freq_domain_pos   = 0,
      .freq_domain_shift = 0,
      .freq_hop          = srs_config::srs_resource::frequency_hopping{.c_srs = 0, .b_srs = 0, .b_hop = 0},
      .grp_or_seq_hop    = srs_config::srs_resource::group_or_sequence_hopping::groupHopping,
      .res_type          = srs_config::srs_resource::semi_persistent,
      .semi_pers_res_type_periodicity_and_offset =
          srs_config::srs_periodicity_and_offset{.type  = srsran::srs_config::srs_periodicity_and_offset::type_t::sl10,
                                                 .value = 30},
      .sequence_id           = 41,
      .spatial_relation_info = srs_config::srs_resource::srs_spatial_relation_info{
          .reference_signal = srs_config::srs_resource::srs_spatial_relation_info::srs_ref_signal{
              .res_id = static_cast<srs_config::srs_res_id>(1), .ul_bwp = static_cast<bwp_id_t>(1)}}});
  // Release.
  dest_pusch_cfg.srs_res.erase(dest_pusch_cfg.srs_res.begin());

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg_present, dest_sp_cell_cfg_ded.ul_config.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg_present);
  // Since its initial setup and no source cell group config was provided srs config must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_set_to_add_mod_list.size(), 1);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_set_to_release_list.size(), 1);

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_to_add_mod_list.size(), 1);
    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.setup().srs_res_to_release_list.size(), 1);
  }
}

TEST(serving_cell_config_converter_test, test_ue_srs_cfg_release_conversion)
{
  auto src_cell_grp_cfg                                                         = make_initial_cell_group_config();
  src_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg = make_initial_srs_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg_present,
            not dest_sp_cell_cfg_ded.ul_config.value().init_ul_bwp.srs_cfg.has_value());
  // SRS Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.srs_cfg.type(), asn1::setup_release_opts::release);
}

TEST(serving_cell_config_converter_test, test_initial_pdsch_serving_cell_cfg_conversion)
{
  auto dest_cell_grp_cfg = make_initial_cell_group_config();

  dest_cell_grp_cfg.cells[0].serv_cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc =
      pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n8;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present, dest_sp_cell_cfg_ded.pdsch_serv_cell_cfg.has_value());
  // Since its initial setup and no source cell group config was provided PDSCH serving cell config must be of setup
  // type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.is_setup());

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().max_mimo_layers_present);
  ASSERT_FALSE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present)
      << "For 8 HARQs, this flag can be avoided";
}

TEST(serving_cell_config_converter_test, test_custom_pdsch_serving_cell_cfg_conversion)
{
  auto src_cell_grp_cfg = make_initial_cell_group_config();

  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  auto& dest_pdsch_serving_cell_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.pdsch_serv_cell_cfg.value();
  // Add new/remove configurations. Need not be valid configuration.
  dest_pdsch_serving_cell_cfg.code_block_group_tx.emplace(pdsch_code_block_group_transmission{
      .max_cbg_per_tb                   = pdsch_code_block_group_transmission::max_code_block_groups_per_tb::n8,
      .code_block_group_flush_indicator = true});

  dest_pdsch_serving_cell_cfg.x_ov_head     = srsran::x_overhead::xoh6;
  dest_pdsch_serving_cell_cfg.nof_harq_proc = srsran::pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n12;
  dest_pdsch_serving_cell_cfg.pucch_cell    = to_serv_cell_index(1);
  dest_pdsch_serving_cell_cfg.processing_type_2_enabled = false;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present, dest_sp_cell_cfg_ded.pdsch_serv_cell_cfg.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.is_setup());

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().code_block_group_tx_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().code_block_group_tx.is_setup());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().max_mimo_layers_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().xoverhead_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().pucch_cell_present);
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().processing_type2_enabled_present);
}

TEST(serving_cell_config_converter_test, test_pdsch_serving_cell_cfg_release_conversion)
{
  auto                      src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.pdsch_serv_cell_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg_present,
            not dest_sp_cell_cfg_ded.pdsch_serv_cell_cfg.has_value());
  // PDSCH Serving Cell Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.pdsch_serving_cell_cfg.type(), asn1::setup_release_opts::release);
}

static csi_meas_config make_test_csi_meas_cfg()
{
  csi_helper::csi_builder_params csi_params{};
  csi_params.pci           = (pci_t)1;
  csi_params.nof_rbs       = 52;
  csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(subcarrier_spacing::kHz15);
  return csi_helper::make_csi_meas_config(csi_params);
}

TEST(serving_cell_config_converter_test, test_initial_csi_meas_cfg_conversion)
{
  auto dest_cell_grp_cfg = make_initial_cell_group_config();

  if (not dest_cell_grp_cfg.cells.begin()->serv_cell_cfg.csi_meas_cfg.has_value()) {
    dest_cell_grp_cfg.cells.begin()->serv_cell_cfg.csi_meas_cfg = make_test_csi_meas_cfg();
  }

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.csi_meas_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg_present, dest_sp_cell_cfg_ded.csi_meas_cfg.has_value());
  // Since its initial setup and no source cell group config was provided csi meas config must be of setup type.
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.csi_meas_cfg.is_setup());

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().nzp_csi_rs_res_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().nzp_csi_rs_res_set_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_im_res_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().csi_im_res_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_im_res_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_im_res_set_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().csi_im_res_set_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_im_res_set_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_ssb_res_set_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().csi_ssb_res_set_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_ssb_res_set_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_res_cfg_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().csi_res_cfg_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_res_cfg_to_release_list.size(), 0);

  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list.size(),
            dest_sp_cell_cfg_ded.csi_meas_cfg.value().csi_report_cfg_list.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_release_list.size(), 0);
}

TEST(serving_cell_config_converter_test, test_custom_csi_meas_cfg_conversion)
{
  auto src_cell_grp_cfg = make_initial_cell_group_config();
  if (not src_cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg.has_value()) {
    src_cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg = make_test_csi_meas_cfg();
  }
  auto& src_meas = src_cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg.value();
  src_meas.nzp_csi_rs_res_list.resize(1);
  src_meas.nzp_csi_rs_res_set_list.resize(1);
  src_meas.csi_res_cfg_list.resize(1);

  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  auto&                     dest_csi_meas_cfg = dest_cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg.value();
  // Add new/remove configurations. Configuration need not be valid.
  // Resource 1.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(1);
  // Resource 2.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(2);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.first_ofdm_symbol_in_td = 8;
  // Resource 3.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_id         = static_cast<nzp_csi_rs_res_id_t>(3);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset = 12;
  // Resource 4.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(4);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.first_ofdm_symbol_in_td = 8;
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset                      = 12;
  // Resource 5.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(5);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.fd_alloc.resize(12);
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.nof_ports    = 2;
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.cdm          = csi_rs_cdm_type::cdm4_FD2_TD2;
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().res_mapping.freq_density = csi_rs_freq_density_type::dot5_odd_RB;
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().csi_res_period           = csi_resource_periodicity::slots320;
  dest_csi_meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset           = 4;
  // Remove.
  dest_csi_meas_cfg.nzp_csi_rs_res_list.erase(dest_csi_meas_cfg.nzp_csi_rs_res_list.begin());

  // Add new/remove configurations. Configuration need not be valid.
  // Resource Set 1.
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_set_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().res_set_id          = static_cast<nzp_csi_rs_res_set_id_t>(1);
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().nzp_csi_rs_res      = {static_cast<nzp_csi_rs_res_id_t>(1),
                                                                          static_cast<nzp_csi_rs_res_id_t>(2),
                                                                          static_cast<nzp_csi_rs_res_id_t>(3),
                                                                          static_cast<nzp_csi_rs_res_id_t>(4)};
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().is_trs_info_present = true;
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.push_back(dest_csi_meas_cfg.nzp_csi_rs_res_set_list[0]);
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().res_set_id               = static_cast<nzp_csi_rs_res_set_id_t>(2);
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().nzp_csi_rs_res           = {static_cast<nzp_csi_rs_res_id_t>(5)};
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().is_repetition_on         = true;
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.back().aperiodic_trigger_offset = 4;
  // Remove.
  dest_csi_meas_cfg.nzp_csi_rs_res_set_list.erase(dest_csi_meas_cfg.nzp_csi_rs_res_set_list.begin());

  // Add new/remove configurations. Configuration need not be valid.
  dest_csi_meas_cfg.csi_im_res_list.push_back(dest_csi_meas_cfg.csi_im_res_list[0]);
  dest_csi_meas_cfg.csi_im_res_list.push_back(dest_csi_meas_cfg.csi_im_res_list[1]);
  dest_csi_meas_cfg.csi_im_res_list.back().res_id = static_cast<csi_im_res_id_t>(1);
  dest_csi_meas_cfg.csi_im_res_list.back().csi_im_res_element_pattern.emplace(
      csi_im_resource::csi_im_resource_element_pattern{
          .pattern_type        = csi_im_resource::csi_im_resource_element_pattern_type::pattern0,
          .subcarrier_location = 2,
          .symbol_location     = 2});
  dest_csi_meas_cfg.csi_im_res_list.back().freq_band_rbs = {4, 4 + 48};
  dest_csi_meas_cfg.csi_im_res_list.back().csi_res_period.emplace(csi_resource_periodicity::slots64);
  dest_csi_meas_cfg.csi_im_res_list.back().csi_res_offset.emplace(11);

  // Add new/remove configurations.
  dest_csi_meas_cfg.csi_im_res_set_list.push_back(dest_csi_meas_cfg.csi_im_res_set_list[0]);
  dest_csi_meas_cfg.csi_im_res_set_list.push_back(dest_csi_meas_cfg.csi_im_res_set_list[0]);
  dest_csi_meas_cfg.csi_im_res_set_list.back().res_set_id        = static_cast<csi_im_res_set_id_t>(1);
  dest_csi_meas_cfg.csi_im_res_set_list.back().csi_ims_resources = {static_cast<csi_im_res_id_t>(1)};

  // Add new configurations. Configuration need not be valid.
  dest_csi_meas_cfg.csi_ssb_res_set_list.push_back(csi_ssb_resource_set{
      .res_set_id = static_cast<csi_ssb_res_set_id_t>(0), .csi_ssb_res_list = {static_cast<ssb_id_t>(0)}});

  // Add new/remove configurations. Configuration need not be valid.
  dest_csi_meas_cfg.csi_res_cfg_list.push_back(dest_csi_meas_cfg.csi_res_cfg_list[0]);
  dest_csi_meas_cfg.csi_res_cfg_list.back().res_cfg_id = static_cast<csi_res_config_id_t>(1);
  csi_resource_config::csi_im_resource_set_list csi_im_res_set{};
  csi_im_res_set.push_back(static_cast<const csi_im_res_set_id_t>(0));
  dest_csi_meas_cfg.csi_res_cfg_list.back().csi_rs_res_set_list = csi_im_res_set;
  dest_csi_meas_cfg.csi_res_cfg_list.push_back(dest_csi_meas_cfg.csi_res_cfg_list[0]);
  dest_csi_meas_cfg.csi_res_cfg_list.back().res_cfg_id = static_cast<csi_res_config_id_t>(2);
  csi_resource_config::nzp_csi_rs_ssb nzp_csi_rs_res_set{};
  nzp_csi_rs_res_set.nzp_csi_rs_res_set_list.push_back(static_cast<const nzp_csi_rs_res_set_id_t>(1));
  dest_csi_meas_cfg.csi_res_cfg_list.back().csi_rs_res_set_list = nzp_csi_rs_res_set;
  dest_csi_meas_cfg.csi_res_cfg_list.push_back(dest_csi_meas_cfg.csi_res_cfg_list[0]);
  dest_csi_meas_cfg.csi_res_cfg_list.back().res_cfg_id = static_cast<csi_res_config_id_t>(3);
  csi_resource_config::nzp_csi_rs_ssb nzp_csi_res_set{};
  nzp_csi_res_set.nzp_csi_rs_res_set_list.push_back(static_cast<const nzp_csi_rs_res_set_id_t>(1));
  nzp_csi_res_set.csi_ssb_res_set_list.push_back(static_cast<const csi_ssb_res_set_id_t>(0));
  dest_csi_meas_cfg.csi_res_cfg_list.back().csi_rs_res_set_list = nzp_csi_res_set;
  dest_csi_meas_cfg.csi_res_cfg_list.back().res_type            = csi_resource_config::resource_type::semiPersistent;
  // Remove.
  dest_csi_meas_cfg.csi_res_cfg_list.erase(dest_csi_meas_cfg.csi_res_cfg_list.begin());

  // Add new/remove configurations. Configuration need not be valid.
  dest_csi_meas_cfg.csi_report_cfg_list.push_back(dest_csi_meas_cfg.csi_report_cfg_list[0]);
  dest_csi_meas_cfg.csi_report_cfg_list.back().report_cfg_id               = static_cast<csi_report_config_id_t>(1);
  dest_csi_meas_cfg.csi_report_cfg_list.back().res_for_channel_meas        = static_cast<csi_res_config_id_t>(2);
  dest_csi_meas_cfg.csi_report_cfg_list.back().csi_im_res_for_interference = static_cast<csi_res_config_id_t>(3);

  csi_report_config::semi_persistent_report_on_pusch report_cfg_type{};
  report_cfg_type.slot_cfg = csi_report_periodicity::slots5;
  report_cfg_type.report_slot_offset_list.push_back(9);
  report_cfg_type.p0_alpha                                     = static_cast<p0_pusch_alphaset_id>(10);
  dest_csi_meas_cfg.csi_report_cfg_list.back().report_cfg_type = report_cfg_type;

  dest_csi_meas_cfg.csi_report_cfg_list.back().report_qty_type =
      csi_report_config::report_quantity_type_t::cri_ri_i1_cqi;
  dest_csi_meas_cfg.csi_report_cfg_list.back().pdsch_bundle_size_for_csi =
      csi_report_config::csi_ri_i1_cqi_pdsch_bundle_size_for_csi::n4;

  dest_csi_meas_cfg.csi_report_cfg_list.back().report_freq_cfg.emplace();
  dest_csi_meas_cfg.csi_report_cfg_list.back().report_freq_cfg.value().cqi_format_ind =
      csi_report_config::cqi_format_indicator::subband_cqi;
  dest_csi_meas_cfg.csi_report_cfg_list.back().report_freq_cfg.value().pmi_format_ind =
      csi_report_config::pmi_format_indicator::subband_pmi;

  dest_csi_meas_cfg.csi_report_cfg_list.back().is_time_restrictions_for_channel_meas_configured      = true;
  dest_csi_meas_cfg.csi_report_cfg_list.back().is_time_restrictions_for_interference_meas_configured = true;

  dest_csi_meas_cfg.csi_report_cfg_list.back().codebook_cfg.emplace();
  auto ant_restriction = codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_value_t(139);
  // '111111'B.
  ant_restriction.all();
  codebook_config::type2::typeii sub_type{};
  sub_type.n1_n2_codebook_subset_restriction_type =
      srsran::codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::four_four;
  sub_type.n1_n2_codebook_subset_restriction_value = ant_restriction;
  sub_type.typeii_ri_restriction                   = bounded_bitset<2>(2);
  // '03'H.
  sub_type.typeii_ri_restriction.from_uint64(0x03);
  codebook_config::type2 type2{};
  type2.sub_type                                                                  = sub_type;
  type2.phase_alphabet_size                                                       = 4;
  type2.subband_amplitude                                                         = true;
  type2.nof_beams                                                                 = 2;
  dest_csi_meas_cfg.csi_report_cfg_list.back().codebook_cfg.value().codebook_type = type2;

  dest_csi_meas_cfg.csi_report_cfg_list.back().is_group_based_beam_reporting_enabled = true;
  dest_csi_meas_cfg.csi_report_cfg_list.back().cqi_table.emplace(cqi_table_t::table1);
  dest_csi_meas_cfg.csi_report_cfg_list.back().subband_size = csi_report_config::subband_size_t::value2;
  dest_csi_meas_cfg.csi_report_cfg_list.back().non_pmi_port_indication.push_back(
      csi_report_config::port_index_for_8_ranks{
          .port_index_type = srsran::csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_4,
          .rank1_x         = 1,
          .rank2_x         = {2, 4},
          .rank3_x         = {3},
          .rank4_x         = {5, 6, 7}});
  // Remove.
  dest_csi_meas_cfg.csi_report_cfg_list.erase(dest_csi_meas_cfg.csi_report_cfg_list.begin());

  dest_csi_meas_cfg.report_trigger_size = 2;

  dest_csi_meas_cfg.aperiodic_trigger_state_list.emplace();
  auto associated_report_cfg_info_list =
      csi_associated_report_config_info{.report_cfg_id   = static_cast<csi_report_config_id_t>(1),
                                        .res_for_channel = csi_associated_report_config_info::csi_ssb_resource_set{1}};
  dest_csi_meas_cfg.aperiodic_trigger_state_list.value().push_back(
      csi_aperiodic_trigger_state{.associated_report_cfg_info_list = {associated_report_cfg_info_list}});

  dest_csi_meas_cfg.semi_persistent_on_pusch_trigger_state_list.emplace();
  dest_csi_meas_cfg.semi_persistent_on_pusch_trigger_state_list.value().push_back(
      csi_semi_persistent_on_pusch_trigger_state{.associated_report_cfg_info = static_cast<csi_report_config_id_t>(1)});

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.csi_meas_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg_present, dest_sp_cell_cfg_ded.csi_meas_cfg.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.csi_meas_cfg.is_setup());
  const auto& setup = rrc_sp_cell_cfg_ded.csi_meas_cfg.setup();

  ASSERT_EQ(setup.nzp_csi_rs_res_to_add_mod_list.size(), 5);
  ASSERT_EQ(setup.nzp_csi_rs_res_to_release_list.size(), 1);

  ASSERT_EQ(setup.nzp_csi_rs_res_set_to_add_mod_list.size(), 2);
  ASSERT_EQ(setup.nzp_csi_rs_res_set_to_release_list.size(), 1);

  ASSERT_EQ(setup.csi_im_res_to_add_mod_list.size(), 2);
  ASSERT_EQ(setup.csi_im_res_to_release_list.size(), 0);

  ASSERT_EQ(setup.csi_im_res_set_to_add_mod_list.size(), 2);
  ASSERT_EQ(setup.csi_im_res_set_to_release_list.size(), 0);

  ASSERT_EQ(setup.csi_ssb_res_set_to_add_mod_list.size(), 1);
  ASSERT_EQ(setup.csi_ssb_res_set_to_release_list.size(), 0);

  ASSERT_EQ(setup.csi_res_cfg_to_add_mod_list.size(), 3);
  ASSERT_EQ(setup.csi_res_cfg_to_release_list.size(), 1);

  ASSERT_EQ(setup.csi_report_cfg_to_add_mod_list.size(), 1);
  ASSERT_EQ(setup.csi_report_cfg_to_release_list.size(), 1);

  ASSERT_TRUE(setup.aperiodic_trigger_state_list_present);
  ASSERT_TRUE(setup.aperiodic_trigger_state_list.is_setup());

  ASSERT_TRUE(setup.semi_persistent_on_pusch_trigger_state_list_present);
  ASSERT_TRUE(setup.semi_persistent_on_pusch_trigger_state_list.is_setup());
}

TEST(serving_cell_config_converter_test, test_csi_meas_cfg_release_conversion)
{
  auto src_cell_grp_cfg = make_initial_cell_group_config();

  if (not src_cell_grp_cfg.cells.begin()->serv_cell_cfg.csi_meas_cfg.has_value()) {
    src_cell_grp_cfg.cells.begin()->serv_cell_cfg.csi_meas_cfg = make_test_csi_meas_cfg();
  }

  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  dest_cell_grp_cfg.cells[0].serv_cell_cfg.csi_meas_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto& rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto& dest_sp_cell_cfg_ded = dest_cell_grp_cfg.cells[0].serv_cell_cfg;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.csi_meas_cfg_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg_present, not dest_sp_cell_cfg_ded.csi_meas_cfg.has_value());
  // CSI-Meas Config is released due to absence in dest cell group config.
  ASSERT_EQ(rrc_sp_cell_cfg_ded.csi_meas_cfg.type(), asn1::setup_release_opts::release);
}
