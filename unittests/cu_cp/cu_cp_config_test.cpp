/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/cu_cp/cu_cp_configuration_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace config_helpers;

TEST(cu_cp_config_test, default_config_is_valid)
{
  srsgnb::srs_cu_cp::cu_cp_configuration cfg = make_default_cu_cp_config();

  // set notifier to non-nullptr to make config valid
  cfg.cu_executor  = reinterpret_cast<task_executor*>(0x1);
  cfg.f1c_notifier = reinterpret_cast<f1c_message_notifier*>(0x1);
  cfg.e1_notifier  = reinterpret_cast<e1_message_notifier*>(0x1);
  cfg.ngc_notifier = reinterpret_cast<srsgnb::srs_cu_cp::ngc_message_notifier*>(0x1);

  ASSERT_TRUE(is_valid_configuration(cfg));
}