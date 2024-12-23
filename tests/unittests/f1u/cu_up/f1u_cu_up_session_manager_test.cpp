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
  five_qi_t five_qi{9}; // no dedicated socket for 5QI=9.
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.2");
  }
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
}

TEST_F(f1u_session_manager_test, five_qi_specific_session_selection)
{
  five_qi_t dedicated_five_qi{7};     // there are dedicated sockets for 5QI=7.
  five_qi_t non_dedicated_five_qi{9}; // no dedicated socket for 5QI=9.
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(dedicated_five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.10.1");
  }
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(non_dedicated_five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(dedicated_five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.10.2");
  }
  {
    const auto& ngu_gw = f1u_session_mngr->get_next_f1u_gateway(dedicated_five_qi);
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.10.1");
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
