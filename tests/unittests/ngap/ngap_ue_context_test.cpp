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

#include "lib/ngap/ue_context/ngap_ue_context.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

// Test class.
class ngap_ue_context_list_test : public ngap_ue_context_list
{
public:
  ngap_ue_context_list_test(timer_factory& timers_, srslog::basic_logger& logger_) :
    ngap_ue_context_list(timers_, logger_)
  {
  }
};

/// Fixture class for NGAP UE context
class ngap_ue_context_test : public ::testing::Test
{
protected:
  ngap_ue_context_test()
  {
    ngap_logger.set_level(srslog::basic_levels::debug);
    srslog::init();
  }
  ~ngap_ue_context_test()
  {
    // flush logger after each test
    srslog::flush();
  }

  ue_index_t generate_random_ue_index()
  {
    return uint_to_ue_index(
        test_rgen::uniform_int<uint64_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1));
  }

  ran_ue_id_t generate_random_ran_ue_id()
  {
    return uint_to_ran_ue_id(
        test_rgen::uniform_int<uint64_t>(ran_ue_id_to_uint(ran_ue_id_t::min), ran_ue_id_to_uint(ran_ue_id_t::max) - 1));
  }

  srslog::basic_logger&        ngap_logger = srslog::fetch_basic_logger("NGAP");
  timer_manager                timer_mng;
  manual_task_worker           ctrl_worker{128};
  timer_factory                timers{timer_mng, ctrl_worker};
  dummy_ngap_cu_cp_ue_notifier ue_notifier;
  ngap_ue_context_list_test    ue_ctxt_list{timers, ngap_logger};
};

TEST_F(ngap_ue_context_test, when_ue_added_then_ue_exists)
{
  ue_index_t  ue_index  = generate_random_ue_index();
  ran_ue_id_t ran_ue_id = generate_random_ran_ue_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  ASSERT_TRUE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_TRUE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ(ue_ctxt_list[ran_ue_id].ue_ids.ran_ue_id, ran_ue_id);
  ASSERT_EQ(ue_ctxt_list[ran_ue_id].ue_ids.ue_index, ue_index);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ran_ue_id, ran_ue_id);
  ASSERT_EQ(ue_ctxt_list[ue_index].ue_ids.ue_index, ue_index);
}

TEST_F(ngap_ue_context_test, when_ue_not_added_then_ue_doesnt_exist)
{
  ue_index_t  ue_index  = generate_random_ue_index();
  ran_ue_id_t ran_ue_id = generate_random_ran_ue_id();

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(ngap_ue_context_test, when_ue_exists_then_removal_succeeds)
{
  ue_index_t  ue_index  = generate_random_ue_index();
  ran_ue_id_t ran_ue_id = generate_random_ran_ue_id();

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  // test removal
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));
}

TEST_F(ngap_ue_context_test, when_ue_is_added_then_next_ue_id_is_increased)
{
  ue_index_t  ue_index  = generate_random_ue_index();
  ran_ue_id_t ran_ue_id = ue_ctxt_list.allocate_ran_ue_id();

  ASSERT_EQ((unsigned)ran_ue_id, (unsigned)ran_ue_id_t::min);

  ue_ctxt_list.add_ue(ue_index, ran_ue_id, ue_notifier);

  // remove ue
  ue_ctxt_list.remove_ue_context(ue_index);

  ASSERT_FALSE(ue_ctxt_list.contains(ran_ue_id));
  ASSERT_FALSE(ue_ctxt_list.contains(ue_index));

  ASSERT_EQ((unsigned)ue_ctxt_list.allocate_ran_ue_id(), (unsigned)ran_ue_id_t::min + 1);
}
