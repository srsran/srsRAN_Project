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
#include "lib/e1ap/cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"

#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Fixture class for E1AP UE context
class e1ap_cu_cp_ue_context_test : public ::testing::Test
{
protected:
  e1ap_cu_cp_ue_context_test()
  {
    e1ap_logger.set_level(srslog::basic_levels::debug);
    srslog::init();
  };
  ~e1ap_cu_cp_ue_context_test()
  {
    // flush logger after each test
    srslog::flush();
  };

  ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(
        test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1));
  };

  timer_manager         timer_mng;
  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("CU-CP-E1");
  manual_task_worker    ctrl_worker{128};
  timer_factory         timers{timer_mng, ctrl_worker};
  e1ap_ue_context_list  ue_ctxt_list{timers, e1ap_logger};
};

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_added_then_ue_exists)
{
  ue_index_t             ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id);

  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_index, ue_index);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  ue_index_t             ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ASSERT_FALSE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(e1ap_cu_cp_ue_context_test, when_unsupported_number_of_ues_addeded_then_ue_not_added)
{
  // Add maximum number of supported UEs
  for (unsigned it = 0; it < MAX_NOF_CU_UES; ++it) {
    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_ctxt_list.next_gnb_cu_cp_ue_e1ap_id();
    ASSERT_NE(cu_cp_ue_e1ap_id, gnb_cu_cp_ue_e1ap_id_t::invalid);
    ue_index_t ue_index = uint_to_ue_index(it);

    ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id);

    ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
    ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  }

  // Try to get another cu_cp_ue_e1ap_id (should fail)
  ASSERT_EQ(ue_ctxt_list.next_gnb_cu_cp_ue_e1ap_id(), gnb_cu_cp_ue_e1ap_id_t::invalid);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  ue_index_t             ue_index         = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ue_ctxt_list.add_ue(ue_index, cu_cp_ue_e1ap_id);

  ue_ctxt_list.remove_ue(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_index_is_old_ue_index_then_ue_index_not_updated)
{
  ue_index_t             old_ue_index     = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ue_ctxt_list.add_ue(old_ue_index, cu_cp_ue_e1ap_id);

  // new ue index to update
  ue_index_t ue_index = old_ue_index;

  // test ue index update
  ue_ctxt_list.update_ue_index(ue_index, old_ue_index);

  ASSERT_TRUE(ue_ctxt_list.contains(old_ue_index));
  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));

  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_index, old_ue_index);
  ASSERT_EQ(ue_ctxt_list[old_ue_index].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[old_ue_index].ue_index, old_ue_index);
}

TEST_F(e1ap_cu_cp_ue_context_test, when_ue_exists_then_ue_index_update_succeeds)
{
  ue_index_t             old_ue_index     = generate_random_ue_index();
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();

  ue_ctxt_list.add_ue(old_ue_index, cu_cp_ue_e1ap_id);

  // new ue index to update
  ue_index_t ue_index = generate_random_ue_index();

  // make sure ue index is not old ue index
  while (ue_index == old_ue_index) {
    ue_index = generate_random_ue_index();
  }

  // test ue index update
  ue_ctxt_list.update_ue_index(ue_index, old_ue_index);

  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  ASSERT_TRUE(ue_ctxt_list.contains(cu_cp_ue_e1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(old_ue_index));

  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[cu_cp_ue_e1ap_id].ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].cu_cp_ue_e1ap_id, cu_cp_ue_e1ap_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_index, ue_index);
}
