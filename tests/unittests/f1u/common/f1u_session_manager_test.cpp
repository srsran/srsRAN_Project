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

#include "f1u_session_manager_test.h"
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
