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

#include "../common/test_helpers.h"
#include "f1ap_cu_test_helpers.h"
#include "lib/f1ap/cu_cp/ue_context/f1ap_cu_ue_context.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"

#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class for F1AP UE context
class f1ap_cu_ue_context_test : public ::testing::Test
{
protected:
  f1ap_cu_ue_context_test() { srslog::init(); };
  ~f1ap_cu_ue_context_test()
  {
    // flush logger after each test
    srslog::flush();
  }

  ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(
        test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1));
  }

  timer_manager        timer_mng;
  manual_task_worker   ctrl_worker{128};
  timer_factory        timers{timer_mng, ctrl_worker};
  f1ap_ue_context_list ue_ctxt_list{timers};
};

TEST_F(f1ap_cu_ue_context_test, when_ue_added_then_ue_exists)
{
  ue_index_t          ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  ASSERT_TRUE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_ue_f1ap_id].cu_ue_f1ap_id, cu_ue_f1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_ue_f1ap_id].ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].cu_ue_f1ap_id, cu_ue_f1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_index, ue_index);
}

TEST_F(f1ap_cu_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  ue_index_t          ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(f1ap_cu_ue_context_test, when_unsupported_number_of_ues_addeded_then_ue_not_added)
{
  // Add maximum number of supported UEs
  for (unsigned it = 0; it < MAX_NOF_UES_PER_DU; ++it) {
    gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
    ASSERT_NE(cu_ue_f1ap_id, gnb_cu_ue_f1ap_id_t::invalid);
    ue_index_t ue_index = uint_to_ue_index(it);

    ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

    ASSERT_TRUE(ue_ctxt_list.contains(cu_ue_f1ap_id));
    ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  }

  // Try to get another cu_ue_f1ap_id (should fail)
  ASSERT_EQ(ue_ctxt_list.next_gnb_cu_ue_f1ap_id(), gnb_cu_ue_f1ap_id_t::invalid);
}

TEST_F(f1ap_cu_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  ue_index_t          ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  // test removal
  ue_ctxt_list.remove_ue(cu_ue_f1ap_id);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);
}
