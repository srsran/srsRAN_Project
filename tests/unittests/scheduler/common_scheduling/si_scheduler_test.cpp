/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../test_utils/scheduler_test_suite.h"
#include "lib/scheduler/common_scheduling/si_scheduler.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/asn1/rrc_nr/ue_cap.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class si_scheduler_setup
{
public:
  si_scheduler_setup(units::bytes sib1_payload_size, const std::optional<si_scheduling_config>& si_sched_cfg) :
    expert_cfg(config_helpers::make_default_scheduler_expert_config()),
    cell_cfg(expert_cfg,
             sched_config_helper::make_default_sched_cell_configuration_request(cell_config_builder_profiles::tdd())),
    pdcch_sch(cell_cfg),
    res_grid(cell_cfg),
    si_sched(cell_cfg, pdcch_sch, sib1_payload_size, si_sched_cfg),
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
  si_scheduler_setup setup{units::bytes{0}, std::nullopt};

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
  si_scheduler_test() : si_scheduler_setup(DEFAULT_SIB1_PAYLOAD_SIZE, DEFAULT_SI_SCHED_CFG) {}

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

TEST_F(si_scheduler_test, when_si_is_updated_then_new_version_is_applied_at_si_change_window)
{
  const units::bytes   new_sib1_len     = DEFAULT_SIB1_PAYLOAD_SIZE + units::bytes{64U};
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};

  // Update SI scheduling.
  si_sched.handle_si_update_request(
      si_scheduling_update_request{to_du_cell_index(0), 1, new_sib1_len, new_si_sched_cfg});

  const unsigned si_ch_wind_len_rfs = static_cast<unsigned>(cell_cfg.dl_cfg_common.bcch_cfg.mod_period_coeff) *
                                      static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned sfn_mod              = next_slot.sfn() % si_ch_wind_len_rfs;
  const unsigned si_change_slot_count = (si_ch_wind_len_rfs - sfn_mod) * next_slot.nof_slots_per_frame();

  const unsigned nof_test_slots      = 2 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  bool           new_version_applied = false;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.version == 1 and not new_version_applied) {
        new_version_applied = true;
        ASSERT_GE(i, si_change_slot_count) << "SI change applied too soon";
      }

      unsigned tbs = sib.pdsch_cfg.codewords[0].tb_size_bytes;
      if (new_version_applied) {
        ASSERT_EQ(sib.version, 1);
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? new_sib1_len.value()
                                                            : new_si_sched_cfg.si_messages[0].msg_len.value());
      } else {
        ASSERT_EQ(sib.version, 0);
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? DEFAULT_SIB1_PAYLOAD_SIZE.value()
                                                            : DEFAULT_SI_SCHED_CFG.si_messages[0].msg_len.value());
      }
    }
  }

  ASSERT_TRUE(new_version_applied);
}
