/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
