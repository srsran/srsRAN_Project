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

#include "lib/scheduler/config/cell_configuration.h"
#include "lib/scheduler/pucch_scheduling/pucch_collision_manager.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

static cell_configuration make_test_cell_configuration(const std::vector<pucch_resource>& ded_res               = {},
                                                       unsigned                           pucch_resource_common = 11)
{
  const auto expert_cfg         = config_helpers::make_default_scheduler_expert_config();
  auto       sched_req          = sched_config_helper::make_default_sched_cell_configuration_request();
  sched_req.ded_pucch_resources = ded_res;
  sched_req.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common = pucch_resource_common;
  return cell_configuration{expert_cfg, sched_req};
}

// For reference, these are the common PUCCH resources with the default configuration:
//
// | r_pucch | Format | Mux index (CS) | 1st hop (RB,sym) | 2nd hop (RB,sym) |
// |---------|--------|----------------|------------------|------------------|
// | 0       | F1     | 0              | [0, 1) [0, 7)    | [51, 52) [7, 14) |
// | 1       | F1     | 6              | [0, 1) [0, 7)    | [51, 52) [7, 14) |
// | 2       | F1     | 0              | [1, 2) [0, 7)    | [50, 51) [7, 14) |
// | 3       | F1     | 6              | [1, 2) [0, 7)    | [50, 51) [7, 14) |
// | 4       | F1     | 0              | [2, 3) [0, 7)    | [49, 50) [7, 14) |
// | 5       | F1     | 6              | [2, 3) [0, 7)    | [49, 50) [7, 14) |
// | 6       | F1     | 0              | [3, 4) [0, 7)    | [48, 49) [7, 14) |
// | 7       | F1     | 6              | [3, 4) [0, 7)    | [48, 49) [7, 14) |
// | 8       | F1     | 0              | [51, 52) [0, 7)  | [0, 1) [7, 14)   |
// | 9       | F1     | 6              | [51, 52) [0, 7)  | [0, 1) [7, 14)   |
// | 10      | F1     | 0              | [50, 51) [0, 7)  | [1, 2) [7, 14)   |
// | 11      | F1     | 6              | [50, 51) [0, 7)  | [1, 2) [7, 14)   |
// | 12      | F1     | 0              | [49, 50) [0, 7)  | [2, 3) [7, 14)   |
// | 13      | F1     | 6              | [49, 50) [0, 7)  | [2, 3) [7, 14)   |
// | 14      | F1     | 0              | [48, 49) [0, 7)  | [3, 4) [7, 14)   |
// | 15      | F1     | 6              | [48, 49) [0, 7)  | [3, 4) [7, 14)   |
// |---------|--------|----------------|------------------|------------------|
TEST(pucch_collision_manager_test, common_resources_dont_collide_with_each_other)
{
  const auto              cell_cfg = make_test_cell_configuration();
  pucch_collision_manager col_manager(cell_cfg);

  slot_point sl(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, 0);
  col_manager.slot_indication(sl);
  for (unsigned r_pucch = 0; r_pucch < pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES; ++r_pucch) {
    auto res = col_manager.alloc_common(sl, r_pucch);
    ASSERT_TRUE(res.has_value());
    res = col_manager.alloc_common(sl, r_pucch);
    ASSERT_FALSE(res.has_value());
    ASSERT_EQ(res.error(), pucch_collision_manager::alloc_failure_reason::PUCCH_COLLISION);
  }
}

TEST(pucch_collision_manager_test, resources_of_different_formats_collide_if_they_overlap_in_time_freq)
{
  const auto              cell_cfg = make_test_cell_configuration({
      pucch_resource{
                       .res_id           = {0, 0},
                       .starting_prb     = 0,
                       .second_hop_prb   = std::nullopt,
                       .nof_symbols      = 14,
                       .starting_sym_idx = 0,
                       .format           = pucch_format::FORMAT_2,
                       .format_params    = pucch_format_2_3_cfg{.nof_prbs = 1},
      },
  });
  pucch_collision_manager col_manager(cell_cfg);

  // Common Res 2..7 and 10..15 don't overlap with the dedicated resources in time/freq, so they don't collide.
  for (unsigned r_pucch = 2; r_pucch != 8; ++r_pucch) {
    ASSERT_FALSE(col_manager.check_common_to_ded_collision(r_pucch, 0));
  }
  for (unsigned r_pucch = 10; r_pucch != 16; ++r_pucch) {
    ASSERT_FALSE(col_manager.check_common_to_ded_collision(r_pucch, 0));
  }

  // Common Res 0, 1, 8, and 9 overlap with the dedicated resource in time/freq, so they collide.
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(0, 0));
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(1, 0));
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(8, 0));
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(9, 0));
}

TEST(pucch_collision_manager_test, multiplexed_resources_collide_if_different_time_freq_grants)
{
  const auto              cell_cfg = make_test_cell_configuration({
      pucch_resource{
                       .res_id           = {0, 0},
                       .starting_prb     = 0,
                       .second_hop_prb   = std::nullopt,
                       .nof_symbols      = 14,
                       .starting_sym_idx = 0,
                       .format           = pucch_format::FORMAT_1,
                       .format_params    = pucch_format_1_cfg{.initial_cyclic_shift = 0, .time_domain_occ = 0},
      },
  });
  pucch_collision_manager col_manager(cell_cfg);

  // Common Res 2..7 and 10..15 don't overlap with the dedicated resources in time/freq, so they don't collide.
  for (unsigned r_pucch = 2; r_pucch != 8; ++r_pucch) {
    ASSERT_FALSE(col_manager.check_common_to_ded_collision(r_pucch, 0));
  }
  for (unsigned r_pucch = 10; r_pucch != 16; ++r_pucch) {
    ASSERT_FALSE(col_manager.check_common_to_ded_collision(r_pucch, 0));
  }

  // Test the collisions between Dedicated Res 0 and the common resources that overlap in time/freq.
  // Common Res 0 overlaps in time/freq and has the same mux index.
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(0, 0));
  // Common Res 9 overlaps in time/freq for the second hop, and even though they have the same format and different mux
  // indices, the time/freq grants are not equal, so they collide.
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(1, 0));
  // Common Res 8 overlaps in time/freq and has the same mux index.
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(8, 0));
  // Common Res 9 overlaps in time/freq for the second hop, and even though they have the same format and different mux
  // indices, the time/freq grants are not equal, so they collide.
  ASSERT_TRUE(col_manager.check_common_to_ded_collision(9, 0));
}

TEST(pucch_collision_manager_test, f0_multiplexed_resources_dont_collide)
{
  std::vector<pucch_resource> ded_res_list;
  for (uint8_t i = 0; i != pucch_constants::format0_initial_cyclic_shift_range.stop(); ++i) {
    ded_res_list.push_back(pucch_resource{
        .res_id           = {i, i},
        .starting_prb     = 0,
        .second_hop_prb   = std::nullopt,
        .nof_symbols      = 2,
        .starting_sym_idx = 0,
        .format           = pucch_format::FORMAT_0,
        .format_params    = pucch_format_0_cfg{.initial_cyclic_shift = i},
    });
  }
  const auto              cell_cfg = make_test_cell_configuration(ded_res_list);
  pucch_collision_manager col_manager(cell_cfg);

  for (unsigned i = 0; i != ded_res_list.size(); ++i) {
    for (unsigned j = i + 1; j != ded_res_list.size(); ++j) {
      ASSERT_FALSE(col_manager.check_ded_to_ded_collision(i, j));
    }
  }
}

TEST(pucch_collision_manager_test, f1_multiplexed_resources_dont_collide)
{
  std::vector<pucch_resource> ded_res_list;
  for (uint8_t i = 0; i != pucch_constants::format1_initial_cyclic_shift_range.stop(); ++i) {
    for (uint8_t j = 0; j != pucch_constants::format1_time_domain_occ_range.stop(); ++j) {
      ded_res_list.push_back(pucch_resource{
          .res_id           = {static_cast<unsigned>(ded_res_list.size()), static_cast<unsigned>(ded_res_list.size())},
          .starting_prb     = 0,
          .second_hop_prb   = std::nullopt,
          .nof_symbols      = 14,
          .starting_sym_idx = 0,
          .format           = pucch_format::FORMAT_1,
          .format_params    = pucch_format_1_cfg{.initial_cyclic_shift = i, .time_domain_occ = j},
      });
    }
  }
  const auto              cell_cfg = make_test_cell_configuration(ded_res_list);
  pucch_collision_manager col_manager(cell_cfg);

  for (unsigned i = 0; i != ded_res_list.size(); ++i) {
    for (unsigned j = i + 1; j != ded_res_list.size(); ++j) {
      ASSERT_FALSE(col_manager.check_ded_to_ded_collision(i, j));
    }
  }
}

TEST(pucch_collision_manager_test, f4_multiplexed_resources_dont_collide)
{
  std::vector<pucch_resource> ded_res_list;
  for (unsigned i = 0; i != 4; ++i) {
    ded_res_list.push_back(pucch_resource{
        .res_id           = {i, i},
        .starting_prb     = 0,
        .second_hop_prb   = std::nullopt,
        .nof_symbols      = 14,
        .starting_sym_idx = 0,
        .format           = pucch_format::FORMAT_4,
        .format_params =
            pucch_format_4_cfg{.occ_length = pucch_f4_occ_len::n4, .occ_index = static_cast<pucch_f4_occ_idx>(i)},
    });
  }
  const auto              cell_cfg = make_test_cell_configuration(ded_res_list);
  pucch_collision_manager col_manager(cell_cfg);

  for (unsigned i = 0; i != ded_res_list.size(); ++i) {
    for (unsigned j = i + 1; j != ded_res_list.size(); ++j) {
      ASSERT_FALSE(col_manager.check_ded_to_ded_collision(i, j));
    }
  }
}

TEST(pucch_collision_manager_test, check_mux_regions_count_for_common_resources)
{
  static constexpr std::array<unsigned, 16> nof_expected_mux_regions = {
      8, // 2 CS
      6, // 3 CS, resources distributed in regions as 3-3-2-3-3-2
      6, // 3 CS, resources distributed in regions as 3-3-2-3-3-2
      8, // 2 CS
      4, // 4 CS
      4, // 4 CS
      4, // 4 CS
      8, // 2 CS
      4, // 4 CS
      4, // 4 CS
      4, // 4 CS
      8, // 2 CS
      4, // 4 CS
      4, // 4 CS
      4, // 4 CS
      4, // 4 CS
  };
  for (unsigned pucch_resource_common = 0; pucch_resource_common != 16; ++pucch_resource_common) {
    const auto              cell_cfg = make_test_cell_configuration({}, pucch_resource_common);
    pucch_collision_manager col_manager(cell_cfg);

    ASSERT_EQ(nof_expected_mux_regions[pucch_resource_common], col_manager.nof_mux_regions());
  }
}

TEST(pucch_collision_manager_test, handles_max_dedicated_resources_with_unique_regions)
{
  auto cell_cfg = make_test_cell_configuration();

  std::vector<pucch_resource> ded_res_list;
  ded_res_list.reserve(pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES);

  // Generate a list with the maximum number of dedicated PUCCH resources in a way that all resources belongs to a
  // different multiplexing region.
  for (unsigned sym = 0, prb = 0; ded_res_list.size() != pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES;) {
    const unsigned res_idx = ded_res_list.size();
    ded_res_list.push_back(pucch_resource{
        .res_id           = {res_idx, res_idx},
        .starting_prb     = prb,
        .second_hop_prb   = std::nullopt,
        .nof_symbols      = 1,
        .starting_sym_idx = static_cast<uint8_t>(sym),
        .format           = pucch_format::FORMAT_2,
        .format_params    = pucch_format_2_3_cfg{.nof_prbs = 1},
    });

    // By using different starting symbols and PRBs for each resource, we ensure that all resources have unique
    // time-frequency allocations, and thus belong to different multiplexing regions.
    // Using Format 2 ensures they are not multiplexed with the common resources either.
    ++sym;
    if (sym == NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
      sym = 0;
      ++prb;
      if (prb == cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()) {
        // No more PRBs available. This should not happen.
        break;
      }
    }
  }
  ASSERT_EQ(ded_res_list.size(), pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES)
      << "Failed to create the maximum number of dedicated PUCCH resources for the test";

  // Overwrite the cell configuration with the dedicated resources.
  cell_cfg.ded_pucch_resources = ded_res_list;
  pucch_collision_manager col_manager(cell_cfg);

  // Only the multiplexing regions of the common resources should be present.
  EXPECT_EQ(8U, col_manager.nof_mux_regions());
}
