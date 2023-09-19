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

#include "lib/scheduler/support/mcs_calculator.h"
#include "lib/scheduler/ue_scheduling/ue_link_adaptation_controller.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_ue_link_adaptation_controller_test
{
public:
  base_ue_link_adaptation_controller_test(optional<float> olla_cqi_inc   = nullopt,
                                          optional<float> max_cqi_offset = nullopt,
                                          optional<float> target_bler    = nullopt) :
    sched_cfg([&]() {
      scheduler_expert_config cfg = config_helpers::make_default_scheduler_expert_config();
      if (olla_cqi_inc.has_value()) {
        cfg.ue.olla_cqi_inc = *olla_cqi_inc;
      }
      if (max_cqi_offset.has_value()) {
        cfg.ue.olla_max_cqi_offset = *max_cqi_offset;
      }
      if (target_bler.has_value()) {
        cfg.ue.olla_dl_target_bler = *target_bler;
      }
      return cfg;
    }())
  {
  }

  scheduler_expert_config       sched_cfg;
  cell_configuration            cell_cfg{sched_cfg, test_helpers::make_default_sched_cell_configuration_request()};
  ue_channel_state_manager      ue_channel_state{sched_cfg.ue, 1};
  ue_link_adaptation_controller controller{cell_cfg, ue_channel_state};

  const pdsch_mcs_table dl_mcs_table = pdsch_mcs_table::qam64;
  const pusch_mcs_table ul_mcs_table = pusch_mcs_table::qam64;
};

class ue_link_adaptation_controller_test : public ::testing::Test, public base_ue_link_adaptation_controller_test
{
public:
};

TEST_F(ue_link_adaptation_controller_test, starts_with_no_snr_offset)
{
  ASSERT_EQ(controller.dl_cqi_offset(), 0);
  ASSERT_EQ(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, acks_increase_offsets)
{
  controller.handle_dl_ack_info(mac_harq_ack_report_status::ack, sch_mcs_index{5}, dl_mcs_table);
  ASSERT_GT(controller.dl_cqi_offset(), 0);

  controller.handle_ul_crc_info(true, sch_mcs_index{5}, pusch_mcs_table::qam64);
  ASSERT_GT(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, nacks_increase_offsets)
{
  controller.handle_dl_ack_info(mac_harq_ack_report_status::nack, sch_mcs_index{5}, dl_mcs_table);
  ASSERT_LT(controller.dl_cqi_offset(), 0);

  controller.handle_ul_crc_info(false, sch_mcs_index{5}, pusch_mcs_table::qam64);
  ASSERT_LT(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, cqi_0_reports_empty_mcs)
{
  // make offset different than zero.
  controller.handle_dl_ack_info(mac_harq_ack_report_status::ack, sch_mcs_index{5}, dl_mcs_table);

  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{0};
  ue_channel_state.handle_csi_report(csi);

  optional<sch_mcs_index> mcs = controller.calculate_dl_mcs(dl_mcs_table);
  ASSERT_FALSE(mcs.has_value());
}

TEST_F(ue_link_adaptation_controller_test, cqi_positive_reports_non_empty_mcs)
{
  // make offset different than zero.
  controller.handle_dl_ack_info(mac_harq_ack_report_status::ack, sch_mcs_index{5}, dl_mcs_table);

  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{test_rgen::uniform_int<uint8_t>(1, 15)};
  ue_channel_state.handle_csi_report(csi);

  optional<sch_mcs_index> mcs = controller.calculate_dl_mcs(dl_mcs_table);
  ASSERT_TRUE(mcs.has_value());
}

class ue_link_adaptation_controller_mcs_derivation_test : public base_ue_link_adaptation_controller_test,
                                                          public ::testing::Test
{
public:
  ue_link_adaptation_controller_mcs_derivation_test() : base_ue_link_adaptation_controller_test(0.1, 15, 0.49) {}
};

TEST_F(ue_link_adaptation_controller_mcs_derivation_test,
       mcs_increases_with_increasing_offset_in_steps_no_larger_than_1)
{
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{5};
  ue_channel_state.handle_csi_report(csi);

  const sch_mcs_index mcs_lb = map_cqi_to_mcs(csi.first_tb_wideband_cqi->value(), dl_mcs_table).value();
  const sch_mcs_index mcs_ub = map_cqi_to_mcs(csi.first_tb_wideband_cqi->value() + 1, dl_mcs_table).value();

  // zero offset case.
  sch_mcs_index mcs_prev = controller.calculate_dl_mcs(dl_mcs_table).value();
  ASSERT_EQ(mcs_prev, mcs_lb);

  // MCS increases with offset, in steps of size equal or less than 1.
  sch_mcs_index mcs = mcs_prev;
  while (mcs != mcs_ub) {
    // Increase offset.
    controller.handle_dl_ack_info(mac_harq_ack_report_status::ack, sch_mcs_index{5}, dl_mcs_table);

    mcs = controller.calculate_dl_mcs(dl_mcs_table).value();
    ASSERT_LE(mcs - mcs_prev, 1U);
    ASSERT_LE(mcs, mcs_ub);
    mcs_prev = mcs;
  }

  ASSERT_GT(controller.dl_cqi_offset(), 1.0F) << "MCS(CQI+offset) == MCS(CQI+1) if offset >= 1";
}
