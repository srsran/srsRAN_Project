/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager_test_helpers.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

ue_manager_test::ue_manager_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &cu_worker;
    cucfg.admission.max_nof_dus   = 6;
    cucfg.admission.max_nof_ues   = cucfg.admission.max_nof_dus * ues_per_du;
    return cucfg;
  }())
{
  test_logger.set_level(srslog::basic_levels::debug);
  ue_mng_logger.set_level(srslog::basic_levels::debug);
  srslog::init();
}

ue_manager_test::~ue_manager_test()
{
  // flush logger after each test
  srslog::flush();
}
