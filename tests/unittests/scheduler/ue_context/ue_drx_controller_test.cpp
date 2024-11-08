/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_context/ue_drx_controller.h"
#include "lib/scheduler/ue_context/ul_logical_channel_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

using msec = std::chrono::milliseconds;

class base_ue_drx_controller_test
{
protected:
  base_ue_drx_controller_test(const std::optional<drx_config>& drx_cfg_, slot_point ul_ccch_slot_ = {}) :
    drx_cfg(drx_cfg_), ul_ccch_slot(ul_ccch_slot_)
  {
  }

  void tick()
  {
    drx.slot_indication(next_slot);
    ++next_slot;
  }

  const subcarrier_spacing   scs = subcarrier_spacing::kHz30;
  std::optional<drx_config>  drx_cfg;
  ul_logical_channel_manager ul_lc_ch_mng;
  slot_point                 ul_ccch_slot;
  ue_drx_controller          drx{scs, msec{64}, drx_cfg, ul_lc_ch_mng, ul_ccch_slot};

  slot_point next_slot{to_numerology_value(scs),
                       test_rgen::uniform_int<unsigned>(0, 10240) * get_nof_slots_per_subframe(scs) - 1};
};

class drx_disabled_ue_drx_controller_test : public base_ue_drx_controller_test, public testing::Test
{
protected:
  drx_disabled_ue_drx_controller_test() : base_ue_drx_controller_test(std::nullopt) {}
};

TEST_F(drx_disabled_ue_drx_controller_test, when_no_drx_config_provided_pdcch_is_always_available)
{
  const unsigned NOF_TESTS = 16;
  for (unsigned i = 0; i != NOF_TESTS; ++i) {
    ASSERT_TRUE(drx.is_pdcch_enabled(next_slot));
    tick();
  }
}

class ue_drx_controller_test : public base_ue_drx_controller_test, public testing::Test
{
protected:
  ue_drx_controller_test() : base_ue_drx_controller_test(drx_config{msec{80}, msec{10}, msec{20}, msec{10}}) {}

  const unsigned period_slots = drx_cfg->long_cycle.count() * get_nof_slots_per_subframe(scs);
  const unsigned offset_slot  = drx_cfg->long_start_offset.count() * get_nof_slots_per_subframe(scs);
  const unsigned on_dur_slots = drx_cfg->on_duration_timer.count() * get_nof_slots_per_subframe(scs);
};

TEST_F(ue_drx_controller_test, when_drx_config_provided_slot_offset_and_on_duration_are_respected)
{
  for (unsigned i = 0; i != period_slots; ++i) {
    bool enabled = drx.is_pdcch_enabled(next_slot);

    const unsigned slot_mod = next_slot.to_uint() % period_slots;
    if (slot_mod >= offset_slot and slot_mod < (offset_slot + on_dur_slots)) {
      ASSERT_TRUE(enabled);
    } else {
      ASSERT_FALSE(enabled);
    }
  }
}
