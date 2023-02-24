/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_test::cu_cp_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_cp_executor = &ctrl_worker;
  cfg.f1ap_notifier  = &f1ap_pdu_notifier;
  cfg.e1ap_notifier  = &e1ap_pdu_notifier;
  cfg.ngap_notifier  = &ngap_amf_notifier;

  cfg.ngap_config.ran_node_name = "srsgnb01";
  cfg.ngap_config.plmn          = "00101";
  cfg.ngap_config.tac           = 7;

  // create and start DUT
  cu_cp_obj = std::make_unique<cu_cp>(std::move(cfg));
  cu_cp_obj->start();
}

cu_cp_test::~cu_cp_test()
{
  // flush logger after each test
  srslog::flush();
  cu_cp_obj->stop();
}
