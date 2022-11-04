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
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace srsgnb;

srs_du::cell_group_config make_initial_cell_group_config()
{
  srs_du::cell_group_config dest_cell_grp_cfg{};
  dest_cell_grp_cfg.spcell_cfg.serv_cell_idx  = to_du_cell_index(0);
  dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded = config_helpers::make_default_initial_ue_serving_cell_config();

  return dest_cell_grp_cfg;
}

TEST(serving_cell_config_converter_test, test_default_initial_ue_pdcch_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

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
  auto                           src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config      dest_cell_grp_cfg{};
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, {});

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

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

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

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
  auto                           src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config      dest_cell_grp_cfg{};
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, {});

  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg_present);
  ASSERT_TRUE(rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded_present);

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

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
  auto& dest_pdsch_cfg = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded.init_dl_bwp.pdsch_cfg.value();
  dest_pdsch_cfg.tci_states.push_back(tci_state{
      .state_id  = static_cast<tci_state_id_t>(1),
      .qcl_type1 = {.ref_sig  = {.type   = qcl_info::reference_signal::reference_signal_type::csi_rs,
                                 .csi_rs = static_cast<nzp_csi_rs_res_id_t>(0)},
                    .qcl_type = qcl_info::qcl_type::type_a},
  });
  dest_pdsch_cfg.tci_states.erase(dest_pdsch_cfg.tci_states.begin());

  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().additional_positions.value() =
      srsgnb::dmrs_additional_positions::pos0;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id0 = 10;
  dest_pdsch_cfg.pdsch_mapping_type_a_dmrs.value().scrambling_id1 = 20;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp_present);
  ASSERT_EQ(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present, dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value());
  ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.is_setup());

  if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.has_value()) {
    if (dest_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.value().pdsch_mapping_type_a_dmrs.has_value()) {
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a_present);
      ASSERT_TRUE(rrc_sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup().dmrs_dl_for_pdsch_map_type_a.is_setup());
    }

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

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

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

  auto rrc_sp_cell_cfg_ded  = rrc_cell_grp_cfg.sp_cell_cfg.sp_cell_cfg_ded;
  auto dest_sp_cell_cfg_ded = dest_cell_grp_cfg.spcell_cfg.spcell_cfg_ded;

  ASSERT_EQ(rrc_sp_cell_cfg_ded.dl_bwp_to_add_mod_list.size(), dest_sp_cell_cfg_ded.dl_bwps.size());
  ASSERT_EQ(rrc_sp_cell_cfg_ded.dl_bwp_to_release_list.size(), 0);
}
