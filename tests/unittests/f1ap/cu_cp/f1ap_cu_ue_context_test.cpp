/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_cu_test_helpers.h"
#include "lib/f1ap/cu_cp/ue_context/f1ap_cu_ue_context.h"
#include "srsran/f1ap/common/f1ap_types.h"
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

  srslog::basic_logger& f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1");
  timer_manager         timer_mng;
  manual_task_worker    ctrl_worker{128};
  timer_factory         timers{timer_mng, ctrl_worker};
  f1ap_ue_context_list  ue_ctxt_list{timers, f1ap_logger};
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
  // Reduce F1AP logger loglevel to warning to reduce console output
  f1ap_logger.set_level(srslog::basic_levels::warning);

  // Add maximum number of supported UEs
  for (unsigned it = 0; it < MAX_NOF_UES_PER_DU; ++it) {
    gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
    ASSERT_NE(cu_ue_f1ap_id, gnb_cu_ue_f1ap_id_t::invalid);
    ue_index_t ue_index = uint_to_ue_index(it);

    ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

    ASSERT_TRUE(ue_ctxt_list.contains(cu_ue_f1ap_id));
    ASSERT_TRUE(ue_ctxt_list.contains(ue_index));
  }

  // Reset F1AP logger loglevel
  f1ap_logger.set_level(srslog::basic_levels::debug);

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
}

TEST_F(f1ap_cu_ue_context_test, when_ue_is_added_then_next_ue_id_is_increased)
{
  ue_index_t          ue_index      = generate_random_ue_index();
  gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();

  ASSERT_EQ((unsigned)cu_ue_f1ap_id, (unsigned)gnb_cu_ue_f1ap_id_t::min);

  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

  // remove ue
  ue_ctxt_list.remove_ue(cu_ue_f1ap_id);

  ASSERT_FALSE(ue_ctxt_list.contains(cu_ue_f1ap_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ((unsigned)ue_ctxt_list.next_gnb_cu_ue_f1ap_id(), (unsigned)gnb_cu_ue_f1ap_id_t::min + 1);
}

// Test is deactivated because it takes ~3min to run, but it passed
#ifdef RUN_MAX_UE_ID_TEST
TEST_F(f1ap_cu_ue_context_test, when_next_ue_id_reaches_max_then_unused_values_are_reused)
{
  // Reduce F1AP logger loglevel to warning to reduce console output
  f1ap_logger.set_level(srslog::basic_levels::warning);

  // Add ue with id 0
  ue_ctxt_list.add_ue(uint_to_ue_index(0), ue_ctxt_list.next_gnb_cu_ue_f1ap_id());

  // increase next cu ue f1ap id
  for (uint64_t it = 1; it < (uint64_t)gnb_cu_ue_f1ap_id_t::max - MAX_NOF_UES_PER_DU + 2; ++it) {
    ASSERT_NE(ue_ctxt_list.next_gnb_cu_ue_f1ap_id(), gnb_cu_ue_f1ap_id_t::invalid);
  }

  // Add maximum number of supported UEs
  unsigned ue_idx = 1;
  for (uint64_t it = (uint64_t)gnb_cu_ue_f1ap_id_t::max - MAX_NOF_UES_PER_DU + 2;
       it <= (uint64_t)gnb_cu_ue_f1ap_id_t::max;
       ++it) {
    gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
    ASSERT_NE(cu_ue_f1ap_id, gnb_cu_ue_f1ap_id_t::invalid);
    ue_index_t ue_index = uint_to_ue_index(ue_idx);

    ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);

    ASSERT_TRUE(ue_ctxt_list.contains(cu_ue_f1ap_id));
    ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

    ue_idx++;
  }

  // Reset F1AP logger loglevel
  f1ap_logger.set_level(srslog::basic_levels::debug);

  // remove an ue from the context list
  gnb_cu_ue_f1ap_id_t rem_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
  ASSERT_TRUE(ue_ctxt_list.contains(rem_ue_id));
  ue_ctxt_list.remove_ue(rem_ue_id);
  ASSERT_FALSE(ue_ctxt_list.contains(rem_ue_id));

  // Next available cu ue f1ap id should be the removed one
  ASSERT_EQ((uint64_t)ue_ctxt_list.next_gnb_cu_ue_f1ap_id(), (uint64_t)rem_ue_id);
}
#endif // RUN_MAX_UE_ID_TEST
