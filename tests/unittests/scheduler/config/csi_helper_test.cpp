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

#include "srsran/ran/tdd/tdd_ul_dl_config_formatters.h"
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_validator.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

void PrintTo(const tdd_ul_dl_config_common& cfg, std::ostream* os)
{
  *os << fmt::format("{}", cfg);
}

} // namespace srsran

class csi_rs_slot_derivation_test : public ::testing::TestWithParam<tdd_ul_dl_config_common>
{
protected:
  csi_rs_slot_derivation_test()
  {
    srsran_assert(csi_helper::derive_valid_csi_rs_slot_offsets(result, nullopt, nullopt, nullopt, tdd_cfg),
                  "Derivation failed");
  }

  tdd_ul_dl_config_common        tdd_cfg = GetParam();
  csi_helper::csi_builder_params result{};
};

TEST_P(csi_rs_slot_derivation_test, csi_rs_slot_offset_fall_in_dl_slots)
{
  static const unsigned ZP_SYMBOL_IDX = 8, MEAS_SYMBOL_IDX = 4, TRACKING_MAX_SYMBOL_IDX = 8;

  ASSERT_GE(get_active_tdd_dl_symbols(tdd_cfg, result.zp_csi_slot_offset, cyclic_prefix::NORMAL).stop(), ZP_SYMBOL_IDX);
  ASSERT_GE(get_active_tdd_dl_symbols(tdd_cfg, result.meas_csi_slot_offset, cyclic_prefix::NORMAL).stop(),
            MEAS_SYMBOL_IDX);
  // Note: Tracking occupies two consecutive slots.
  ASSERT_GE(get_active_tdd_dl_symbols(tdd_cfg, result.tracking_csi_slot_offset, cyclic_prefix::NORMAL).stop(),
            TRACKING_MAX_SYMBOL_IDX);
  ASSERT_GE(get_active_tdd_dl_symbols(tdd_cfg, result.tracking_csi_slot_offset + 1, cyclic_prefix::NORMAL).stop(),
            TRACKING_MAX_SYMBOL_IDX);
}

TEST_P(csi_rs_slot_derivation_test, csi_rs_slot_offsets_do_not_collide)
{
  // Note: ZP and NZP-CSI-RS slots are always in different symbols.
  ASSERT_NE(result.zp_csi_slot_offset, result.tracking_csi_slot_offset);
  ASSERT_NE(result.zp_csi_slot_offset, result.tracking_csi_slot_offset + 1);
  ASSERT_NE(result.meas_csi_slot_offset, result.tracking_csi_slot_offset);
  ASSERT_NE(result.meas_csi_slot_offset, result.tracking_csi_slot_offset + 1);
}

TEST_P(csi_rs_slot_derivation_test, generated_csi_meas_config_validation)
{
  serving_cell_config cell_cfg = config_helpers::create_default_initial_ue_serving_cell_config();
  result.nof_rbs               = 52;
  cell_cfg.csi_meas_cfg        = make_csi_meas_config(result);

  config_validators::validate_csi_meas_cfg(cell_cfg, tdd_cfg);
}

INSTANTIATE_TEST_SUITE_P(
    csi_helper_test,
    csi_rs_slot_derivation_test,
    // clang-format off
    ::testing::Values(tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}, nullopt},
                      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 9, 3, 0}, nullopt},
                      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 9, 2, 0}, nullopt}));
// clang-format on
