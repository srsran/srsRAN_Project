/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_cu_test_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

f1ap_cu_test::f1ap_cu_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  f1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  f1c = create_f1ap(f1c_pdu_notifier, du_processor_notifier, f1c_du_mgmt_notifier);
}

f1ap_cu_test::~f1ap_cu_test()
{
  // flush logger after each test
  srslog::flush();
}
