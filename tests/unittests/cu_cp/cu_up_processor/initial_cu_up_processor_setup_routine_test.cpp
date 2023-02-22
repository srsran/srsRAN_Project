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

#include "cu_up_processor_routine_manager_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Note: Check if UE ID is valid is done by caller. Injection of invalid ue_index results in assertion.

class initial_cu_up_processor_setup_routine_test : public cu_up_processor_routine_manager_test
{
protected:
  void start_procedure(bool cu_cp_e1_setup_outcome)
  {
    e1ap_conn_notifier.set_cu_cp_e1_setup_outcome(cu_cp_e1_setup_outcome);

    routine_mng->start_initial_cu_up_processor_setup_routine();
  }

  bool was_initial_cu_up_processor_setup_successful() const
  {
    if (context.cu_up_name == "srsCU-UP") {
      return true;
    }
    return false;
  }
};

TEST_F(initial_cu_up_processor_setup_routine_test, when_setup_successful_then_context_updated)
{
  // Start initial CU-UP processor setup routine.
  this->start_procedure(true);

  ASSERT_TRUE(was_initial_cu_up_processor_setup_successful());
}

TEST_F(initial_cu_up_processor_setup_routine_test, when_setup_unsuccessful_then_context_not_updated)
{
  // Start initial CU-UP processor setup routine.
  this->start_procedure(false);

  ASSERT_FALSE(was_initial_cu_up_processor_setup_successful());
}
