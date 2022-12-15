/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/du_ue/du_bearer.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace srsgnb;
using namespace srs_du;

TEST(du_ue_bearer_manager_test, when_no_drbs_allocated_lcid_is_min)
{
  du_ue_bearer_manager bearers;
  ASSERT_EQ(bearers.allocate_lcid(), LCID_MIN_DRB);
}

TEST(du_ue_bearer_manager_test, when_all_drbs_are_allocated_then_no_lcid_is_available)
{
  std::vector<unsigned> lcids(MAX_NOF_DRBS);
  std::iota(lcids.begin(), lcids.end(), (unsigned)LCID_MIN_DRB);
  std::shuffle(lcids.begin(), lcids.end(), test_rgen::get());

  du_ue_bearer_manager bearers;
  for (unsigned i = 0; i != MAX_NOF_DRBS; ++i) {
    drb_id_t drb_id = (drb_id_t)((unsigned)drb_id_t::drb1 + i);
    bearers.add_drb(drb_id, uint_to_lcid(lcids[i]), {});
  }
  ASSERT_FALSE(bearers.allocate_lcid().has_value());
}

TEST(du_ue_bearer_manager_test, when_there_is_a_hole_in_allocated_lcids_then_allocate_lcid_method_finds_it)
{
  std::vector<unsigned> lcids(MAX_NOF_DRBS);
  std::iota(lcids.begin(), lcids.end(), (unsigned)LCID_MIN_DRB);
  std::shuffle(lcids.begin(), lcids.end(), test_rgen::get());
  lcid_t lcid_hole = uint_to_lcid(lcids.back());
  lcids.pop_back();
  std::shuffle(lcids.begin(), lcids.end(), test_rgen::get());

  du_ue_bearer_manager bearers;
  for (unsigned i = 0; i != lcids.size(); ++i) {
    drb_id_t drb_id = (drb_id_t)((unsigned)drb_id_t::drb1 + i);
    bearers.add_drb(drb_id, uint_to_lcid(lcids[i]), {});
  }
  optional<lcid_t> found_lcid = bearers.allocate_lcid();
  ASSERT_TRUE(found_lcid.has_value());
  ASSERT_EQ(*found_lcid, lcid_hole);
}
