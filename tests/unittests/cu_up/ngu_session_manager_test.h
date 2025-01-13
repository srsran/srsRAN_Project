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

#include "cu_up_test_helpers.h"
#include "lib/cu_up/ngu_session_manager_impl.h"
#include <gtest/gtest.h>

namespace srsran::srs_cu_up {

/// Fixture base class for PDU session manager tests
class ngu_session_manager_test : public ::testing::Test
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    unsigned nof_gws = 2;
    for (unsigned i = 0; i < nof_gws; i++) {
      auto        ngu_gw = std::make_unique<dummy_gtpu_gateway>();
      std::string addr   = fmt::format("127.0.0.{}", 1 + i);
      ngu_gw->set_bind_address(addr);
      ngu_gws.push_back(std::move(ngu_gw));
    }

    // todo init ngu session manager
    ngu_session_mngr = std::make_unique<ngu_session_manager_impl>(ngu_gws);
  }

  void TearDown() override
  {
    ngu_gws.clear();
    ngu_session_mngr.reset();

    // flush logger after each test
    srslog::flush();
  }

protected:
  std::unique_ptr<ngu_session_manager>               ngu_session_mngr;
  std::vector<std::unique_ptr<gtpu_tnl_pdu_session>> ngu_gws;
};

} // namespace srsran::srs_cu_up
