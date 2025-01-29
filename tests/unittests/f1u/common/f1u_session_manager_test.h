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

#pragma once

#include "lib/f1u/split_connector/f1u_session_manager_impl.h"
#include "tests/unittests/cu_up/cu_up_test_helpers.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

namespace srsran::srs_cu_up {

/// Fixture base class for PDU session manager tests
class f1u_session_manager_test : public ::testing::Test
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    {
      unsigned nof_gws = 2;
      for (unsigned i = 0; i < nof_gws; i++) {
        auto        f1u_gw = std::make_unique<dummy_gtpu_gateway>();
        std::string addr   = fmt::format("127.0.0.{}", 1 + i);
        f1u_gw->set_bind_address(addr);
        f1u_sessions.default_gw_sessions.push_back(std::move(f1u_gw));
      }
    }

    {
      five_qi_t five_qi{7};
      unsigned  nof_gws = 2;
      for (unsigned i = 0; i < nof_gws; i++) {
        auto        f1u_gw = std::make_unique<dummy_gtpu_gateway>();
        std::string addr   = fmt::format("127.0.10.{}", 1 + i);
        f1u_gw->set_bind_address(addr);
        f1u_sessions.five_qi_gw_sessions[five_qi].push_back(std::move(f1u_gw));
      }
    }

    // todo init ngu session manager
    f1u_session_mngr = std::make_unique<f1u_session_manager_impl>(f1u_sessions);
  }

  void TearDown() override
  {
    f1u_sessions.default_gw_sessions.clear();
    f1u_sessions.five_qi_gw_sessions.clear();
    f1u_session_mngr.reset();

    // flush logger after each test
    srslog::flush();
  }

protected:
  std::unique_ptr<f1u_session_manager> f1u_session_mngr;
  f1u_session_maps                     f1u_sessions;
  dummy_inner_f1u_bearer               f1u_bearer;
};

} // namespace srsran::srs_cu_up
