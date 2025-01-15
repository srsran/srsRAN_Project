/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  ASSERT_NE(ngu_session_mngr, nullptr);
}

TEST_F(ngu_session_manager_test, rr_session_selection)
{
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.2");
  }
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
