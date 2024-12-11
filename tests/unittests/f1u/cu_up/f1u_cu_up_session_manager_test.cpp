/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_cu_up_session_manager_test.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

TEST_F(f1u_session_manager_test, mngr_creation)
{
  ASSERT_NE(f1u_session_mngr, nullptr);
}

TEST_F(f1u_session_manager_test, rr_session_selection)
{
  {
    const auto&           ngu_gw = f1u_session_mngr->get_next_f1u_gateway();
    expected<std::string> ip_addr;
    ip_addr = ngu_gw.get_cu_bind_address();
    ASSERT_TRUE(ip_addr.has_value());
    ASSERT_EQ(ip_addr.value(), "127.0.0.1");
  }
  {
    const auto&           ngu_gw = f1u_session_mngr->get_next_f1u_gateway();
    expected<std::string> ip_addr;
    ip_addr = ngu_gw.get_cu_bind_address();
    ASSERT_TRUE(ip_addr.has_value());
    ASSERT_EQ(ip_addr.value(), "127.0.0.2");
  }
  {
    const auto&           ngu_gw = f1u_session_mngr->get_next_f1u_gateway();
    expected<std::string> ip_addr;
    ip_addr = ngu_gw.get_cu_bind_address();
    ASSERT_TRUE(ip_addr.has_value());
    ASSERT_EQ(ip_addr.value(), "127.0.0.1");
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
