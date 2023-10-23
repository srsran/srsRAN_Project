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

#include "lib/scheduler/cell/resource_grid.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

std::vector<scs_specific_carrier> test_carriers()
{
  std::vector<scs_specific_carrier> ret;
  // 15 kHz.
  ret.emplace_back();
  ret.back().scs               = subcarrier_spacing::kHz15;
  ret.back().offset_to_carrier = 0;
  ret.back().carrier_bandwidth = 52;
  // 120 kHz.
  ret.emplace_back();
  ret.back().scs               = subcarrier_spacing::kHz120;
  ret.back().offset_to_carrier = 0;
  ret.back().carrier_bandwidth = 275;
  return ret;
}

TEST(carrier_subslot_resource_grid_test, test_all)
{
  std::vector<scs_specific_carrier> carrier_cfgs = test_carriers();

  // Wideband Carrier, 15kHz case.
  {
    carrier_subslot_resource_grid carrier_grid(carrier_cfgs[0]);
    TESTASSERT_EQ(subcarrier_spacing::kHz15, carrier_grid.scs());
    TESTASSERT_EQ(52, carrier_grid.nof_rbs());
    TESTASSERT_EQ(0, carrier_grid.offset());
    crb_interval lims{0, 52};
    TESTASSERT(carrier_grid.rb_dims() == lims);
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{0, 14}, lims));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 5}, crb_interval{2, 5}));
    TESTASSERT(not carrier_grid.all_set(ofdm_symbol_range{0, 14}, lims));

    carrier_grid.fill({2, 5}, crb_interval{2, 4});
    TESTASSERT(carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{1, 3}));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{1, 2}));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{0, 1}, crb_interval{1, 3}));
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{2, 5}, crb_interval{2, 4}));
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{2, 4}, crb_interval{2, 3}));
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{3, 5}, crb_interval{3, 4}));
    TESTASSERT(not carrier_grid.all_set(ofdm_symbol_range{1, 5}, crb_interval{1, 4}));
    TESTASSERT(not carrier_grid.all_set(ofdm_symbol_range{2, 6}, crb_interval{2, 5}));

    carrier_grid.fill(ofdm_symbol_range{0, 14}, lims);
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{0, 14}, lims));
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{0, 13}, {0, 51}));
    TESTASSERT(carrier_grid.all_set(ofdm_symbol_range{1, 14}, {1, 52}));

    carrier_grid.clear();
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{1, 3}));
  }

  // Narrowband Carrier, 15kHz case.
  {
    scs_specific_carrier carrier_cfg = carrier_cfgs[0];
    carrier_cfg.offset_to_carrier    = 10;
    carrier_cfg.carrier_bandwidth    = 20;
    carrier_subslot_resource_grid carrier_grid(carrier_cfg);
    TESTASSERT_EQ(subcarrier_spacing::kHz15, carrier_grid.scs());
    TESTASSERT_EQ(20, carrier_grid.nof_rbs());
    TESTASSERT_EQ(10, carrier_grid.offset());
    crb_interval lims{10, 30};
    TESTASSERT(carrier_grid.rb_dims() == lims);
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{0, 14}, lims));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 5}, crb_interval{12, 15}));

    carrier_grid.fill({2, 5}, crb_interval{12, 14});
    TESTASSERT(carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{11, 13}));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{11, 12}));
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{0, 1}, crb_interval{11, 13}));

    carrier_grid.clear();
    TESTASSERT(not carrier_grid.collides(ofdm_symbol_range{2, 3}, crb_interval{11, 13}));
  }
}

TEST(cell_resource_grid_test, test_all)
{
  std::vector<scs_specific_carrier> carrier_cfgs = test_carriers();

  // Wide BWP, 15 kHz case.
  {
    cell_slot_resource_grid cell_grid{carrier_cfgs};
    bwp_configuration       bwp_cfg{};
    bwp_cfg.scs  = srsran::subcarrier_spacing::kHz15;
    bwp_cfg.crbs = {0, 52};

    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info(bwp_cfg, {0, 14}, {0, 52})));

    bwp_sch_grant_info grant{bwp_cfg, {2, 14}, {5, 10}};
    cell_grid.fill(grant);
    TESTASSERT(cell_grid.collides(subcarrier_spacing::kHz15, {0, 14}, crb_interval{0, 52}));
    TESTASSERT(not cell_grid.collides(subcarrier_spacing::kHz15, {0, 14}, crb_interval{0, 5}));
    TESTASSERT(cell_grid.collides(subcarrier_spacing::kHz15, {2, 3}, crb_interval{0, 6}));
    TESTASSERT(not cell_grid.collides(subcarrier_spacing::kHz15, {0, 2}, crb_interval{0, 6}));

    cell_grid.clear();
    TESTASSERT(not cell_grid.collides(subcarrier_spacing::kHz15, {2, 3}, crb_interval{0, 6}));
  }

  // Narrow BWP, 15 kHz case.
  {
    cell_slot_resource_grid cell_grid{carrier_cfgs};
    bwp_configuration       bwp_cfg{};
    bwp_cfg.scs  = srsran::subcarrier_spacing::kHz15;
    bwp_cfg.crbs = {10, 30};

    TESTASSERT(not cell_grid.collides(subcarrier_spacing::kHz15, {0, 14}, crb_interval{0, 52}));

    bwp_sch_grant_info grant{bwp_cfg, {2, 14}, {5, 10}};
    cell_grid.fill(grant);
    TESTASSERT(cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 14}, {0, 20}}));
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 14}, {0, 5}}));
    TESTASSERT(cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {2, 3}, {0, 6}}));
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 2}, {0, 6}}));

    cell_grid.clear();
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {2, 3}, {0, 6}}));
  }

  // Wide BWP, 120 kHz case.
  {
    cell_slot_resource_grid cell_grid{carrier_cfgs};
    bwp_configuration       bwp_cfg{};
    bwp_cfg.scs  = srsran::subcarrier_spacing::kHz120;
    bwp_cfg.crbs = {10, 275};

    TESTASSERT(not cell_grid.collides(subcarrier_spacing::kHz120, {0, 14}, crb_interval{0, 265}));

    bwp_sch_grant_info grant{bwp_cfg, {2, 14}, {5, 200}};
    cell_grid.fill(grant);
    TESTASSERT(cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 14}, {10, 30}}));
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 14}, {0, 5}}));
    TESTASSERT(cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {2, 3}, {0, 30}}));
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {0, 2}, {0, 30}}));

    cell_grid.clear();
    TESTASSERT(not cell_grid.collides(bwp_sch_grant_info{bwp_cfg, {2, 3}, {0, 30}}));
  }
}

/// Test allocation of resources in a cell resource grid
TEST(pusch_resource_allocation_test, test_all)
{
  scheduler_expert_config sched_cfg = config_helpers::make_default_scheduler_expert_config();
  cell_configuration      cell_cfg{sched_cfg, test_helpers::make_default_sched_cell_configuration_request()};
  cell_resource_allocator res_grid_alloc{cell_cfg};
  bwp_configuration       bwp_cfg{};
  bwp_cfg.crbs = {0, 52};
  bwp_cfg.scs  = cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list[0].scs;

  slot_point sl_tx{0, 0};

  // Action 1: New slot
  res_grid_alloc.slot_indication(sl_tx);

  // Test: resource allocator operator[] returns a slot_allocator pointing at the correct slot
  TESTASSERT_EQ(sl_tx, res_grid_alloc.slot_tx());
  TESTASSERT_EQ(sl_tx, res_grid_alloc[0].slot);
  TESTASSERT_EQ(sl_tx + 1, res_grid_alloc[1].slot);

  // Test: No allocations made yet
  TESTASSERT(res_grid_alloc[0].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).none());
  TESTASSERT(res_grid_alloc[0].result.ul.puschs.empty());

  // Action 2: Allocate PUSCH grant in current slot_tx
  bwp_sch_grant_info ul_grant{bwp_cfg, {0, 14}, {1, 5}};
  res_grid_alloc[0].ul_res_grid.fill(ul_grant);
  res_grid_alloc[0].result.ul.puschs.emplace_back();

  // Test: Allocated PUSCH was registered in the cell resource grid for slot_tx
  TESTASSERT(res_grid_alloc[0].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).any());
  TESTASSERT_EQ(res_grid_alloc[0].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).count(), ul_grant.prbs.length());
  TESTASSERT_EQ(1, res_grid_alloc[0].result.ul.puschs.size());

  // Action 3: Allocate PUSCH grant in slot_tx + 1
  bwp_sch_grant_info ul_grant2{bwp_cfg, {0, 14}, {4, 20}};
  res_grid_alloc[1].ul_res_grid.fill(ul_grant2);
  res_grid_alloc[1].result.ul.puschs.emplace_back();

  // Test: Allocated PUSCH was registered in the cell resource grid for slot_tx + 1
  TESTASSERT(res_grid_alloc[1].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).any());
  TESTASSERT_EQ(res_grid_alloc[1].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).count(), ul_grant2.prbs.length());
  TESTASSERT_EQ(1, res_grid_alloc[1].result.ul.puschs.size());

  // Action 4: New slot
  res_grid_alloc.slot_indication(++sl_tx);

  // Test: Current slot_tx allocations match the ones done with "slot_tx + 1" in the previous slot
  TESTASSERT_EQ(res_grid_alloc[0].ul_res_grid.used_crbs(bwp_cfg, {0, 14}).count(), ul_grant2.prbs.length());
}
