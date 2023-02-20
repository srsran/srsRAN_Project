/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace config_helpers;

TEST(cu_cp_config_test, default_config_is_valid)
{
  srsran::srs_cu_cp::cu_cp_configuration cfg = make_default_cu_cp_config();
  ASSERT_TRUE(is_valid_configuration(cfg));
}