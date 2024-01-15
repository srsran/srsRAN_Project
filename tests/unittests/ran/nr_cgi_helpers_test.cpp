/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/nr_cgi_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(nr_cgi_helpers_test, nr_cell_identity_validity)
{
  nr_cell_global_id_t valid_cgi;
  valid_cgi.mcc      = 1;
  valid_cgi.mnc      = 1;
  valid_cgi.plmn     = "00101";
  valid_cgi.plmn_hex = "00f110";

  ASSERT_TRUE(srsran::config_helpers::is_valid(valid_cgi));

  nr_cell_global_id_t invalid_cgi;
  invalid_cgi.mcc      = 0;
  invalid_cgi.mnc      = 0;
  invalid_cgi.plmn     = "00000";
  invalid_cgi.plmn_hex = "00f000";

  ASSERT_FALSE(srsran::config_helpers::is_valid(invalid_cgi));
}

TEST(nr_cgi_helpers_test, make_nr_cell_identity_test)
{
  ASSERT_EQ(srsran::config_helpers::make_nr_cell_identity(411, 32, 0), 6576);
}
