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

#include "lib/scheduler/cell/scheduler_prb.h"
#include "lib/scheduler/support/rb_helper.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(prb_grant_test, test_all)
{
  // TEST: default ctor
  vrb_alloc grant;
  TESTASSERT(grant.is_type1());
  TESTASSERT(grant.type1().length() == 0);

  // TEST: assignment of RBGs
  rbg_bitmap rbgs(18);
  rbgs.set(1);
  grant = rbgs;
  TESTASSERT(grant.is_type0() and grant.type0().count() == 1);

  // TEST: assignment of PRBs
  vrb_interval vrb_interv(2, 5);
  grant = vrb_interv;
  TESTASSERT(grant.is_type1() and grant.type1().length() == 3);

  // TEST: non-default ctor
  vrb_alloc grant2(vrb_interv), grant3(rbgs);
  TESTASSERT(grant2.is_type1() and grant2.type1().length() == 3);
  TESTASSERT(grant3.is_type0() and grant3.type0().count() == 1);

  // TEST: copy ctor
  vrb_alloc grant4(grant2), grant5(grant3);
  TESTASSERT(grant4.is_type1() and grant4.type1().length() == 3);
  TESTASSERT(grant5.is_type0() and grant5.type0().count() == 1);

  // TEST: copy assignment
  grant = grant3;
  TESTASSERT(grant.is_type0() and grant.type0().count() == 1);
  grant = grant2;
  TESTASSERT(grant.is_type1() and grant.type1().length() == 3);

  // TEST: formatting
  TESTASSERT_EQ("[2..5)", fmt::format("{}", grant2));
  TESTASSERT_EQ("0x10000", fmt::format("0x{:x}", grant3));
}

TEST(bwp_rb_bitmap, test_all)
{
  bwp_rb_bitmap rb_bitmap(275, 0, true);

  TESTASSERT(rb_bitmap.P() == 16);
  TESTASSERT(rb_bitmap.rbgs().none());
  TESTASSERT(rb_bitmap.prbs().none());
  TESTASSERT(rb_bitmap.prbs().size() == 275 and rb_bitmap.nof_prbs() == 275);
  TESTASSERT(rb_bitmap.rbgs().size() == 18 and rb_bitmap.nof_rbgs() == 18);

  rb_bitmap.add(prb_interval{0, 1});
  TESTASSERT(rb_bitmap.prbs().count() == 1 and rb_bitmap.prbs().test(0));
  TESTASSERT(rb_bitmap.rbgs().count() == 1 and rb_bitmap.rbgs().test(0));
  rb_bitmap.add(prb_interval{2, 4});
  TESTASSERT(rb_bitmap.prbs().count() == 3 and rb_bitmap.prbs().test(2) and not rb_bitmap.prbs().test(1));
  TESTASSERT(rb_bitmap.rbgs().count() == 1 and rb_bitmap.rbgs().test(0));

  prb_bitmap prbs(rb_bitmap.nof_prbs());
  prbs.set(1);
  prbs.set(2);
  prbs.set(15);
  rb_bitmap.add(prbs);
  TESTASSERT(rb_bitmap.prbs().count() == 5 and rb_bitmap.prbs().test(1) and rb_bitmap.prbs().test(15));
  TESTASSERT(rb_bitmap.rbgs().count() == 1 and rb_bitmap.rbgs().test(0));
  prbs.set(16);
  rb_bitmap |= prbs;
  TESTASSERT(rb_bitmap.prbs().count() == 6 and rb_bitmap.prbs().test(16));
  TESTASSERT(rb_bitmap.rbgs().count() == 2 and rb_bitmap.rbgs().test(1));

  rbg_bitmap rbgs(rb_bitmap.nof_rbgs());
  rbgs.set(3);
  rbgs.set(17);
  rb_bitmap |= rbgs;
  TESTASSERT(rb_bitmap.prbs().count() == (6 + 16 + 3) and rb_bitmap.prbs().test(rb_bitmap.nof_prbs() - 1));
  TESTASSERT(rb_bitmap.rbgs().count() == 4 and rb_bitmap.rbgs().test(3) and rb_bitmap.rbgs().test(17));
  rbgs.set(0);
  rb_bitmap |= rbgs;
  TESTASSERT(rb_bitmap.prbs().count() == (16 + 1 + 16 + 3) and rb_bitmap.prbs().test(rb_bitmap.nof_prbs() - 1));
  TESTASSERT(rb_bitmap.rbgs().count() == 4 and rb_bitmap.rbgs().test(3) and rb_bitmap.rbgs().test(17));

  // TEST: collides operator
  TESTASSERT(rb_bitmap.collides(rbgs));
  TESTASSERT(rb_bitmap.collides(vrb_interval{0, 2}));
}

TEST(bwp_rb_bitmap, search)
{
  bwp_rb_bitmap rb_bitmap(275, 0, true);

  crb_interval prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), 5);
  TESTASSERT(prbs == prb_interval(0, 5));
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), rb_bitmap.prbs().size());
  TESTASSERT(prbs == prb_interval(0, rb_bitmap.prbs().size()));

  rb_bitmap |= prb_interval{1, 5};
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), rb_bitmap.prbs().size());
  TESTASSERT(prbs == prb_interval(5, rb_bitmap.prbs().size()));

  rb_bitmap |= prb_interval{16, 32};
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), rb_bitmap.prbs().size());
  TESTASSERT(prbs == prb_interval(32, rb_bitmap.prbs().size()));

  rb_bitmap |= prb_interval{270, 275};
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), rb_bitmap.prbs().size());
  TESTASSERT(prbs == prb_interval(32, 270));
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), 1);
  TESTASSERT(prbs == prb_interval(0, 1));
  prbs = rb_helper::find_empty_interval_of_length(rb_bitmap.prbs(), 5);
  TESTASSERT(prbs == prb_interval(5, 10));
}
