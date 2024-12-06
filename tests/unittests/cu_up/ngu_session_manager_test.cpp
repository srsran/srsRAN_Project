/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngu_session_manager_test.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

TEST_F(ngu_session_manager_test, mngr_creation)
{
  ngu_session_mngr = std::make_unique<ngu_session_manager_impl>(ngu_gws);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
