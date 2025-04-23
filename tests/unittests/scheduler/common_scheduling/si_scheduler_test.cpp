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

#include "../test_utils/scheduler_test_suite.h"
#include "lib/scheduler/common_scheduling/si_scheduler.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/support/paging_helpers.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/asn1/rrc_nr/ue_cap.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/scheduler/result/dci_info.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

static sched_cell_configuration_request_message
make_sched_configuration_request(units::bytes                               sib1_payload_size,
                                 const std::optional<si_scheduling_config>& si_sched_cfg)
{
  sched_cell_configuration_request_message msg = sched_config_helper::make_default_sched_cell_configuration_request();
  msg.sib1_payload_size                        = sib1_payload_size;
  msg.si_scheduling                            = si_sched_cfg;
  return msg;
}

class si_scheduler_setup
{
public:
  si_scheduler_setup(const sched_cell_configuration_request_message& msg) :
    expert_cfg(config_helpers::make_default_scheduler_expert_config()),
    cell_cfg(expert_cfg,
             sched_config_helper::make_default_sched_cell_configuration_request(cell_config_builder_profiles::tdd())),
    pdcch_sch(cell_cfg),
    res_grid(cell_cfg),
    si_sched(cell_cfg, pdcch_sch, msg),
    logger(srslog::fetch_basic_logger("SCHED"))
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    // Sets the first slot.
    run_slot();
  }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    res_grid.slot_indication(next_slot);
    pdcch_sch.slot_indication(next_slot);
    si_sched.run_slot(res_grid);

    test_scheduler_result_consistency(cell_cfg, res_grid);

    ++next_slot;
  }

  const scheduler_expert_config expert_cfg;
  const cell_configuration      cell_cfg;
  pdcch_resource_allocator_impl pdcch_sch;
  cell_resource_allocator       res_grid;
  si_scheduler                  si_sched;
  srslog::basic_logger&         logger;

  slot_point next_slot{to_numerology_value(cell_cfg.scs_common),
                       test_rgen::uniform_int<unsigned>(0, 1024 * get_nof_slots_per_subframe(cell_cfg.scs_common))};
};

TEST(no_si_scheduler_test, when_no_si_is_provided_then_nothing_is_scheduled)
{
  si_scheduler_setup setup{make_sched_configuration_request(units::bytes{0}, std::nullopt)};

  const unsigned nof_slots = 100;

  for (unsigned i = 0; i != nof_slots; ++i) {
    setup.run_slot();

    ASSERT_TRUE(setup.res_grid[0].result.dl.bc.sibs.empty());
    ASSERT_TRUE(setup.res_grid[0].result.dl.dl_pdcchs.empty());
  }
}

class si_scheduler_test : public si_scheduler_setup, public testing::Test
{
protected:
  si_scheduler_test() :
    si_scheduler_setup(make_sched_configuration_request(DEFAULT_SIB1_PAYLOAD_SIZE, DEFAULT_SI_SCHED_CFG))
  {
  }

  static constexpr units::bytes     DEFAULT_SIB1_PAYLOAD_SIZE{128};
  static const si_scheduling_config DEFAULT_SI_SCHED_CFG;
};

const si_scheduling_config si_scheduler_test::DEFAULT_SI_SCHED_CFG{
    {{si_message_scheduling_config{units::bytes{64}, 16}}},
    10};

TEST_F(si_scheduler_test, when_sib1_is_cfg_then_sib1_gets_scheduled)
{
  const unsigned nof_test_slots =
      DEFAULT_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();

  std::array<bool, 2> si_scheduled{false, false};
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      ASSERT_EQ(sib.version, 0);
      if (sib.si_indicator == sib_information::sib1) {
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, DEFAULT_SIB1_PAYLOAD_SIZE.value());
        si_scheduled[0] = true;
      } else {
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, DEFAULT_SI_SCHED_CFG.si_messages[0].msg_len.value());
        ASSERT_EQ(sib.si_msg_index.value(), 0);
        si_scheduled[1] = true;
      }
    }
  }

  std::array<bool, 2> expected{true, true};
  ASSERT_EQ(si_scheduled, expected);
}

TEST_F(si_scheduler_test, when_si_is_updated_then_new_version_is_applied_at_si_change_window_boundary)
{
  const units::bytes   new_sib1_len     = DEFAULT_SIB1_PAYLOAD_SIZE + units::bytes{64U};
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};

  // Update SI scheduling.
  si_sched.handle_si_update_request(
      si_scheduling_update_request{to_du_cell_index(0), 1, new_sib1_len, new_si_sched_cfg});

  const unsigned si_ch_wind_len_rfs = static_cast<unsigned>(cell_cfg.dl_cfg_common.bcch_cfg.mod_period_coeff) *
                                      static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned sfn_mod = (next_slot + res_grid.max_dl_slot_alloc_delay).sfn() % si_ch_wind_len_rfs;
  const unsigned si_change_min_count =
      (si_ch_wind_len_rfs - sfn_mod) * next_slot.nof_slots_per_frame() - next_slot.slot_index();

  const unsigned          nof_test_slots = 2 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  unsigned                last_version   = 0;
  std::optional<unsigned> window_version;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    slot_point current_slot = next_slot;
    run_slot();

    if (current_slot.sfn() % si_ch_wind_len_rfs == 0 and current_slot.slot_index() == 0) {
      // Detected SI change window start.
      window_version = std::nullopt;
      logger.info("TEST: New window starting at {}", current_slot);
    }

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (window_version.has_value()) {
        ASSERT_EQ(sib.version, window_version.value())
            << "SI version cannot change in the middle of a SI change window";
      } else {
        window_version = sib.version;
        ASSERT_GE(sib.version, last_version) << "SI version cannot decrease";
        last_version = sib.version;

        if (sib.version == 1) {
          ASSERT_GE(i, si_change_min_count) << "SI change applied too soon";
        }
      }

      unsigned tbs = sib.pdsch_cfg.codewords[0].tb_size_bytes;
      if (sib.version == 1) {
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? new_sib1_len.value()
                                                            : new_si_sched_cfg.si_messages[0].msg_len.value())
            << "New SI payload length not applied";
      } else {
        ASSERT_EQ(sib.version, 0);
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? DEFAULT_SIB1_PAYLOAD_SIZE.value()
                                                            : DEFAULT_SI_SCHED_CFG.si_messages[0].msg_len.value());
      }
    }
  }

  ASSERT_EQ(last_version, 1);
}

TEST_F(si_scheduler_test, when_si_is_updated_all_ues_in_rrc_idle_get_notified_exactly_once)
{
  const paging_slot_helper slot_helper(
      cell_cfg, make_sched_configuration_request(DEFAULT_SIB1_PAYLOAD_SIZE, DEFAULT_SI_SCHED_CFG));
  const units::bytes   new_sib1_len     = DEFAULT_SIB1_PAYLOAD_SIZE + units::bytes{64U};
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};

  // Update SI scheduling.
  si_sched.handle_si_update_request(
      si_scheduling_update_request{to_du_cell_index(0), 1, new_sib1_len, new_si_sched_cfg});

  const unsigned si_ch_wind_len_rfs = static_cast<unsigned>(cell_cfg.dl_cfg_common.bcch_cfg.mod_period_coeff) *
                                      static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle);

  const unsigned                   nof_test_slots      = 2 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  bool                             new_version_applied = false;
  static constexpr unsigned        total_nof_ue_ids    = 1024;
  bounded_bitset<total_nof_ue_ids> notified_ue_ids(total_nof_ue_ids);
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    slot_point current_slot = next_slot;
    run_slot();

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.version == 1) {
        new_version_applied = true;
      }
    }

    for (const auto& pdcch : res_grid[0].result.dl.dl_pdcchs) {
      if (pdcch.dci.type != srsran::dci_dl_rnti_config_type::p_rnti_f1_0) {
        continue;
      }

      const auto& dci = pdcch.dci.p_rnti_f1_0;
      if (dci.short_messages_indicator != srsran::dci_1_0_p_rnti_configuration::payload_info::short_messages) {
        continue;
      }
      ASSERT_EQ(dci.short_messages, 0x80);

      // Notifications shouldn't be sent after the new version is applied.
      ASSERT_FALSE(new_version_applied);

      const unsigned paging_frame_offset  = cell_cfg.dl_cfg_common.pcch_cfg.paging_frame_offset;
      const auto     drx_cycle            = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle);
      const auto     nof_pf_per_drx_cycle = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.nof_pf);
      const auto     nof_po_per_pf        = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.ns);
      const unsigned N                    = drx_cycle / nof_pf_per_drx_cycle;
      const unsigned t_div_n              = drx_cycle / N;
      for (unsigned ue_id = 0; ue_id < total_nof_ue_ids; ++ue_id) {
        // Check for paging frame.
        // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
        const unsigned ue_id_mod_n = ue_id % N;
        if (((current_slot.sfn() + paging_frame_offset) % drx_cycle) != (t_div_n * ue_id_mod_n)) {
          continue;
        }

        // Index (i_s), indicating the index of the PO.
        // i_s = floor (UE_ID/N) mod Ns.
        const unsigned i_s = (ue_id / N) % nof_po_per_pf;
        if (slot_helper.is_paging_slot(current_slot, i_s)) {
          ASSERT_FALSE(notified_ue_ids.test(ue_id));
          notified_ue_ids.set(ue_id);
          logger.debug("TEST: UE ID {} notified at {}", ue_id, current_slot);
        }
      }
    }
  }

  ASSERT_TRUE(notified_ue_ids.all());
}
