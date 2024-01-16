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
#include "srsran/asn1/rrc_nr/cell_group_config.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

static srs_du::cell_group_config make_initial_cell_group_config()
{
  srs_du::cell_group_config dest_cell_grp_cfg{};
  dest_cell_grp_cfg.mcg_cfg = config_helpers::make_initial_mac_cell_group_config();
  return dest_cell_grp_cfg;
}

TEST(mac_cell_group_config_converter_test, test_default_initial_sr_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  const auto& rrc_mcg_cfg  = rrc_cell_grp_cfg.mac_cell_group_cfg;
  const auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;

  if (not dest_mcg_cfg.scheduling_request_config.empty()) {
    ASSERT_TRUE(rrc_mcg_cfg.sched_request_cfg_present);

    ASSERT_EQ(rrc_mcg_cfg.sched_request_cfg.sched_request_to_add_mod_list.size(),
              dest_mcg_cfg.scheduling_request_config.size());
    ASSERT_EQ(rrc_mcg_cfg.sched_request_cfg.sched_request_to_release_list.size(), 0);
  }
}

TEST(mac_cell_group_config_converter_test, test_custom_sr_cfg_conversion)
{
  const auto&               src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup.
  auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;
  dest_mcg_cfg.scheduling_request_config.push_back(scheduling_request_to_addmod{
      .sr_id = static_cast<scheduling_request_id>(1), .prohibit_timer = sr_prohib_timer::ms1, .max_tx = sr_max_tx::n8});
  dest_mcg_cfg.scheduling_request_config.push_back(
      scheduling_request_to_addmod{.sr_id          = static_cast<scheduling_request_id>(2),
                                   .prohibit_timer = sr_prohib_timer::ms128,
                                   .max_tx         = sr_max_tx::n16});
  dest_mcg_cfg.scheduling_request_config.erase(dest_mcg_cfg.scheduling_request_config.begin());

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  auto rrc_mcg_cfg = rrc_cell_grp_cfg.mac_cell_group_cfg;

  if (not dest_mcg_cfg.scheduling_request_config.empty()) {
    ASSERT_TRUE(rrc_mcg_cfg.sched_request_cfg_present);

    ASSERT_EQ(rrc_mcg_cfg.sched_request_cfg.sched_request_to_add_mod_list.size(), 2);
    ASSERT_EQ(rrc_mcg_cfg.sched_request_cfg.sched_request_to_release_list.size(), 1);
  }
}

TEST(mac_cell_group_config_converter_test, test_custom_bsr_cfg_conversion)
{
  const auto&               src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup. Assume BSR Config is already set.
  auto& dest_mcg_cfg                             = dest_cell_grp_cfg.mcg_cfg;
  dest_mcg_cfg.bsr_cfg.value().periodic_timer    = srsran::periodic_bsr_timer::sf2560;
  dest_mcg_cfg.bsr_cfg.value().lc_sr_delay_timer = logical_channel_sr_delay_timer::sf1024;
  dest_mcg_cfg.bsr_cfg.value().retx_timer        = srsran::retx_bsr_timer::sf5120;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  auto rrc_mcg_cfg = rrc_cell_grp_cfg.mac_cell_group_cfg;

  ASSERT_TRUE(rrc_mcg_cfg.bsr_cfg_present);
  ASSERT_EQ(rrc_mcg_cfg.bsr_cfg.retx_bsr_timer, asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf5120);
  ASSERT_EQ(rrc_mcg_cfg.bsr_cfg.periodic_bsr_timer, asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf2560);
  ASSERT_TRUE(rrc_mcg_cfg.bsr_cfg.lc_ch_sr_delay_timer_present);
  ASSERT_EQ(rrc_mcg_cfg.bsr_cfg.lc_ch_sr_delay_timer, asn1::rrc_nr::bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf1024);
}

TEST(mac_cell_group_config_converter_test, test_default_initial_tag_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  const auto& rrc_mcg_cfg  = rrc_cell_grp_cfg.mac_cell_group_cfg;
  const auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;

  if (not dest_mcg_cfg.tag_config.empty()) {
    ASSERT_TRUE(rrc_mcg_cfg.tag_cfg_present);

    ASSERT_EQ(rrc_mcg_cfg.tag_cfg.tag_to_add_mod_list.size(), dest_mcg_cfg.tag_config.size());
    ASSERT_EQ(rrc_mcg_cfg.tag_cfg.tag_to_release_list.size(), 0);
  }
}

TEST(mac_cell_group_config_converter_test, test_custom_tag_cfg_conversion)
{
  const auto&               src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup.
  auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;
  dest_mcg_cfg.tag_config.push_back(tag{.tag_id = static_cast<tag_id_t>(1), .ta_timer = time_alignment_timer::ms2560});
  dest_mcg_cfg.tag_config.push_back(tag{.tag_id = static_cast<tag_id_t>(2), .ta_timer = time_alignment_timer::ms1280});
  dest_mcg_cfg.tag_config.erase(dest_mcg_cfg.tag_config.begin());

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  auto rrc_mcg_cfg = rrc_cell_grp_cfg.mac_cell_group_cfg;

  if (not dest_mcg_cfg.tag_config.empty()) {
    ASSERT_TRUE(rrc_mcg_cfg.tag_cfg_present);

    ASSERT_EQ(rrc_mcg_cfg.tag_cfg.tag_to_add_mod_list.size(), 2);
    ASSERT_EQ(rrc_mcg_cfg.tag_cfg.tag_to_release_list.size(), 1);
  }
}

TEST(mac_cell_group_config_converter_test, test_default_initial_phr_cfg_conversion)
{
  auto                           dest_cell_grp_cfg = make_initial_cell_group_config();
  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, {}, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  const auto& rrc_mcg_cfg  = rrc_cell_grp_cfg.mac_cell_group_cfg;
  const auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;

  if (dest_mcg_cfg.phr_cfg.has_value()) {
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg_present);
    // Since its initial setup and no source mac cell group config was provided PHR Config must be of setup type.
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg.is_setup());
  }
}

TEST(mac_cell_group_config_converter_test, test_custom_phr_cfg_conversion)
{
  const auto&               src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Add new configuration to be setup.
  auto& dest_mcg_cfg                                  = dest_cell_grp_cfg.mcg_cfg;
  dest_mcg_cfg.phr_cfg.value().periodic_timer         = phr_periodic_timer::sf100;
  dest_mcg_cfg.phr_cfg.value().prohibit_timer         = phr_prohibit_timer::sf20;
  dest_mcg_cfg.phr_cfg.value().power_factor_change    = phr_tx_power_factor_change::db3;
  dest_mcg_cfg.phr_cfg.value().multiple_phr           = true;
  dest_mcg_cfg.phr_cfg.value().dummy                  = true;
  dest_mcg_cfg.phr_cfg.value().phr_type_to_other_cell = true;
  dest_mcg_cfg.phr_cfg.value().phr_mode               = phr_mode_other_cg::virtual_;

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  auto rrc_mcg_cfg = rrc_cell_grp_cfg.mac_cell_group_cfg;

  if (dest_mcg_cfg.phr_cfg.has_value()) {
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg_present);
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg.is_setup());

    ASSERT_EQ(rrc_mcg_cfg.phr_cfg.setup().phr_periodic_timer, asn1::rrc_nr::phr_cfg_s::phr_periodic_timer_opts::sf100);
    ASSERT_EQ(rrc_mcg_cfg.phr_cfg.setup().phr_prohibit_timer, asn1::rrc_nr::phr_cfg_s::phr_prohibit_timer_opts::sf20);
    ASSERT_EQ(rrc_mcg_cfg.phr_cfg.setup().phr_tx_pwr_factor_change,
              asn1::rrc_nr::phr_cfg_s::phr_tx_pwr_factor_change_opts::db3);
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg.setup().multiple_phr);
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg.setup().dummy);
    ASSERT_TRUE(rrc_mcg_cfg.phr_cfg.setup().phr_type2_other_cell);
    ASSERT_EQ(rrc_mcg_cfg.phr_cfg.setup().phr_mode_other_cg,
              asn1::rrc_nr::phr_cfg_s::phr_mode_other_cg_opts::virtual_value);
  }
}

TEST(serving_cell_config_converter_test, test_phr_cfg_release_conversion)
{
  const auto&               src_cell_grp_cfg = make_initial_cell_group_config();
  srs_du::cell_group_config dest_cell_grp_cfg{src_cell_grp_cfg};
  // Remove PHR configuration to be setup.
  auto& dest_mcg_cfg = dest_cell_grp_cfg.mcg_cfg;
  dest_mcg_cfg.phr_cfg.reset();

  asn1::rrc_nr::cell_group_cfg_s rrc_cell_grp_cfg;
  srs_du::calculate_cell_group_config_diff(rrc_cell_grp_cfg, src_cell_grp_cfg, dest_cell_grp_cfg);

  ASSERT_TRUE(rrc_cell_grp_cfg.mac_cell_group_cfg_present);

  auto rrc_mcg_cfg = rrc_cell_grp_cfg.mac_cell_group_cfg;

  ASSERT_TRUE(rrc_mcg_cfg.phr_cfg_present);
  // PHR Config is released due to absence in dest mac cell group config.
  ASSERT_EQ(rrc_mcg_cfg.phr_cfg.type(), asn1::setup_release_opts::release);
}
