/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generators.h"
#include "lib/scheduler/cell/resource_grid.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Test allocation of resources in a cell resource grid
void test_pusch_resource_allocation()
{
  cell_configuration cell_cfg{make_cell_cfg_req()};
  cell_resource_grid res_grid_alloc{cell_cfg};

  slot_point sl_tx{0, 0};

  // Action 1: New slot
  res_grid_alloc.slot_indication(sl_tx);

  // Test: resource allocator operator[] returns a slot_allocator pointing at the correct slot
  TESTASSERT_EQ(sl_tx, res_grid_alloc.slot_tx());
  TESTASSERT_EQ(sl_tx, res_grid_alloc[0].slot);
  TESTASSERT_EQ(sl_tx + 1, res_grid_alloc[1].slot);

  // Test: No allocations made yet
  TESTASSERT(res_grid_alloc[0].ul_prbs.none());
  TESTASSERT(res_grid_alloc[0].result.ul.puschs.empty());

  // Action 2: Allocate PUSCH grant in current slot_tx
  prb_interval ul_grant{1, 5};
  res_grid_alloc[0].ul_prbs |= ul_grant;
  res_grid_alloc[0].result.ul.puschs.emplace_back();

  // Test: Allocated PUSCH was registered in the cell resource grid for slot_tx
  TESTASSERT(res_grid_alloc[0].ul_prbs.any());
  TESTASSERT_EQ(res_grid_alloc[0].ul_prbs.count(), ul_grant.length());
  TESTASSERT_EQ(1, res_grid_alloc[0].result.ul.puschs.size());

  // Action 3: Allocate PUSCH grant in slot_tx + 1
  prb_interval ul_grant2{4, 20};
  res_grid_alloc[1].ul_prbs |= ul_grant2;
  res_grid_alloc[1].result.ul.puschs.emplace_back();

  // Test: Allocated PUSCH was registered in the cell resource grid for slot_tx + 1
  TESTASSERT(res_grid_alloc[1].ul_prbs.any());
  TESTASSERT_EQ(res_grid_alloc[1].ul_prbs.count(), ul_grant2.length());
  TESTASSERT_EQ(1, res_grid_alloc[1].result.ul.puschs.size());

  // Action 4: New slot
  res_grid_alloc.slot_indication(++sl_tx);

  // Test: Current slot_tx allocations match the ones done with "slot_tx + 1" in the previous slot
  TESTASSERT_EQ(res_grid_alloc[0].ul_prbs.count(), ul_grant2.length());
}

int main()
{
  test_pusch_resource_allocation();
}
