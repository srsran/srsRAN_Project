/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/du_manager/converters/asn1_cell_group_config_helpers.h"
#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace srsgnb;

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
      .id             = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(0),
      .p0             = 0,
      .p0_pusch_alpha = alpha::alpha1});
  cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
      .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .rs = static_cast<ssb_id_t>(0)});
  cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
      .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(0),
      .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .sri_p0_pusch_alphaset_id     = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(0),
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

  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().additional_positions.value() = dmrs_additional_positions::pos0;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id0               = 10;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id1               = 20;

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
  dest_pucch_cfg.pucch_res_set.back().pucch_res_id_list.emplace_back(1);
  // Remove first element.
  dest_pucch_cfg.pucch_res_set.erase(dest_pucch_cfg.pucch_res_set.begin());

  // >>> PUCCH resource 2.
  pucch_resource res_basic{.res_id                 = 4,
                           .starting_prb           = 0,
                           .second_hop_prb         = 0,
                           .intraslot_freq_hopping = false,
                           .format                 = pucch_format::FORMAT_2};
  res_basic.format                        = pucch_format::FORMAT_2;
  res_basic.format_1.initial_cyclic_shift = 0;
  res_basic.format_1.nof_symbols          = 1;
  res_basic.format_1.starting_sym_idx     = 13;
  res_basic.format_1.time_domain_occ      = 0;
  dest_pucch_cfg.pucch_res_list.push_back(res_basic);

  pucch_resource res_basic2{.res_id                 = 5,
                            .starting_prb           = 40,
                            .second_hop_prb         = 50,
                            .intraslot_freq_hopping = true,
                            .format                 = pucch_format::FORMAT_3};
  res_basic.format                        = pucch_format::FORMAT_3;
  res_basic.format_1.initial_cyclic_shift = 0;
  res_basic.format_1.nof_symbols          = 2;
  res_basic.format_1.starting_sym_idx     = 7;
  res_basic.format_1.time_domain_occ      = 0;
  dest_pucch_cfg.pucch_res_list.push_back(res_basic2);

  // Remove first element.
  dest_pucch_cfg.pucch_res_list.erase(dest_pucch_cfg.pucch_res_list.begin());

  dest_pucch_cfg.dl_data_to_ul_ack.push_back(2);
  dest_pucch_cfg.dl_data_to_ul_ack.push_back(3);

  // Release default one.
  dest_pucch_cfg.format_1_common_param.reset();

  dest_pucch_cfg.format_2_common_param.emplace();
  dest_pucch_cfg.format_2_common_param.value().max_c_rate = pucch_common_all_formats::max_code_rate::dot_25;
  dest_pucch_cfg.format_2_common_param.value().simultaneous_harq_ack_csi = true;

  dest_pucch_cfg.format_3_common_param.emplace();
  dest_pucch_cfg.format_3_common_param.value().max_c_rate = pucch_common_all_formats::max_code_rate::dot_08;
  dest_pucch_cfg.format_3_common_param.value().nof_slots  = pucch_common_all_formats::num_of_slots::n2;

  dest_pucch_cfg.format_4_common_param.emplace();
  dest_pucch_cfg.format_4_common_param.value().max_c_rate      = pucch_common_all_formats::max_code_rate::dot_60;
  dest_pucch_cfg.format_4_common_param.value().additional_dmrs = true;

  // >>> SR Resource.
  dest_pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 2,
                                                                          .sr_id        = uint_to_sched_req_id(1),
                                                                          .period       = sr_periodicity::sl_80,
                                                                          .offset       = 0,
                                                                          .pucch_res_id = 1});

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

    ASSERT_EQ(rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.size(), 2);
    auto& pucch_res1 = rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[0];
    ASSERT_EQ(pucch_res1.pucch_res_id, 4);
    auto& pucch_res2 = rrc_sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[1];
    ASSERT_EQ(pucch_res2.pucch_res_id, 5);
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
      .id             = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(1),
      .p0             = 2,
      .p0_pusch_alpha = alpha::alpha05});
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
          .sri_p0_pusch_alphaset_id     = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(1),
          .closed_loop_idx = pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0});
  // Erase/Release.
  dest_pusch_cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.erase(
      dest_pusch_cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.begin());

  dest_pusch_cfg.pusch_td_alloc_list.push_back(pusch_time_domain_resource_allocation{
      .k2 = 4, .map_type = srsgnb::sch_mapping_type::typeB, .symbols = ofdm_symbol_range{2, 12}});

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
          srs_config::srs_periodicity_and_offset{.type  = srsgnb::srs_config::srs_periodicity_and_offset::type_t::sl10,
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
