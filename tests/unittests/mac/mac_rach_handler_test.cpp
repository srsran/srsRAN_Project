/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/mac/mac_sched/mac_rach_handler.h"
#include "lib/mac/rnti_manager.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_rach_handler_test : public ::testing::Test
{
protected:
  mac_rach_handler_test() :
    logger(srslog::fetch_basic_logger("MAC")),
    params(cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30)),
    sched_cfg([this]() {
      auto cfg = sched_config_helper::make_default_sched_cell_configuration_request(params);
      // Leave some preambles for CFRA.
      cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb = 56;
      return cfg;
    }()),
    handler(sched, rnti_mng, logger),
    cell_handler(handler.add_cell(sched_cfg))
  {
  }

  mac_rach_indication make_rach_indication(uint8_t preamble_id) const
  {
    mac_rach_indication rach;
    rach.slot_rx                                 = {to_numerology_value(params.scs_common), 0};
    mac_rach_indication::rach_occasion& occ      = rach.occasions.emplace_back();
    occ.frequency_index                          = 0;
    occ.slot_index                               = 0;
    occ.start_symbol                             = 0;
    mac_rach_indication::rach_preamble& preamble = occ.preambles.emplace_back();
    preamble.index                               = preamble_id;
    return rach;
  }

  uint8_t create_cb_preamble() const
  {
    return test_rgen::uniform_int(0U,
                                  sched_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb - 1U);
  }

  uint8_t create_cf_preamble() const
  {
    return test_rgen::uniform_int<unsigned>(
        sched_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb,
        sched_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles - 1U);
  }

  srslog::basic_logger&                    logger;
  test_helpers::dummy_mac_scheduler        sched;
  rnti_manager                             rnti_mng;
  cell_config_builder_params               params;
  sched_cell_configuration_request_message sched_cfg;

  mac_rach_handler            handler;
  mac_cell_rach_handler_impl& cell_handler;
};

TEST_F(mac_rach_handler_test, when_cb_rach_detected_then_tc_rnti_is_allocated_and_forwarded_to_sched)
{
  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(create_cb_preamble());
  cell_handler.handle_rach_indication(rach);

  // RACH is forwarded to scheduler with an allocated TC-RNTI.
  ASSERT_TRUE(sched.last_rach_ind.has_value());
  ASSERT_EQ(sched.last_rach_ind.value().occasions.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].preamble_id, rach.occasions[0].preambles[0].index);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti, to_rnti(0x4601));
}

TEST_F(mac_rach_handler_test, when_cf_rach_detected_then_allocated_crnti_is_used)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_TRUE(sched.last_rach_ind.has_value());
  ASSERT_EQ(sched.last_rach_ind.value().occasions.size(), 1);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].preamble_id, cfra_preamble);
  ASSERT_EQ(sched.last_rach_ind.value().occasions[0].preambles[0].tc_rnti, to_rnti(0x5555));
}

TEST_F(mac_rach_handler_test, when_cf_preamble_detected_but_no_cfra_ue_exists_then_rach_is_not_forwarded)
{
  uint8_t cfra_preamble = create_cf_preamble();

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_FALSE(sched.last_rach_ind.has_value());
}

TEST_F(mac_rach_handler_test, when_cf_preamble_is_deallocated_then_cf_rach_is_not_forwarded)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));
  cell_handler.handle_cfra_deallocation(to_du_ue_index(0));

  // Report detected RACH.
  mac_rach_indication rach = make_rach_indication(cfra_preamble);
  cell_handler.handle_rach_indication(rach);

  ASSERT_FALSE(sched.last_rach_ind.has_value());
}

TEST_F(mac_rach_handler_test, when_same_cf_preamble_is_allocated_multiple_times_then_allocation_fails)
{
  uint8_t cfra_preamble = create_cf_preamble();
  ASSERT_TRUE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(0), to_rnti(0x5555)));
  ASSERT_FALSE(cell_handler.handle_cfra_allocation(cfra_preamble, to_du_ue_index(1), to_rnti(0x5556)));
}
