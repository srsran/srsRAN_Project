/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_routine_manager_test_helpers.h"
#include "tests/unittests/rrc/rrc_ue_test_messages.h"
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_routine_manager_test::cu_cp_routine_manager_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  ue_task_sched  = std::make_unique<dummy_du_processor_ue_task_scheduler>(timers, ctrl_worker);
  cu_cp_notifier = std::make_unique<dummy_du_processor_cu_cp_notifier>(ngap_ue_removal_handler, &ue_mng);

  drb_cfg                                                    = {};
  drb_cfg.five_qi_config[uint_to_five_qi(9)]                 = {};
  drb_cfg.five_qi_config[uint_to_five_qi(9)].pdcp.tx.sn_size = pdcp_sn_size::size12bits;
  drb_cfg.five_qi_config[uint_to_five_qi(9)].pdcp.rx.sn_size = pdcp_sn_size::size12bits;

  drb_cfg.five_qi_config[uint_to_five_qi(7)]                 = {};
  drb_cfg.five_qi_config[uint_to_five_qi(7)].pdcp.tx.sn_size = pdcp_sn_size::size12bits;
  drb_cfg.five_qi_config[uint_to_five_qi(7)].pdcp.rx.sn_size = pdcp_sn_size::size12bits;

  rrc_ue_up_resource_manager = std::make_unique<up_resource_manager_impl>(drb_cfg);
  // create routine manager
  routine_mng = std::make_unique<cu_cp_routine_manager>(ue_mng, default_security_indication, cu_cp_logger);

  init_security_config();
}

cu_cp_routine_manager_test::~cu_cp_routine_manager_test()
{
  // flush logger after each test
  srslog::flush();
}

void cu_cp_routine_manager_test::init_security_config()
{
  const char* k_enc_cstr = "4ea96992c8c7e82977231ad001309062ae9f31ead90a4d0842af6cd25cb44dc4";
  const char* k_int_cstr = "aeeb5e0ae02c6188ecb1625c4a9e022fdfc2a1fc845b44b44443ac9a3bda667c";

  // Pack hex strings into srsgnb types
  security::sec_key k_enc = make_sec_key(k_enc_cstr);
  security::sec_key k_int = make_sec_key(k_int_cstr);

  // Create expected SRB1 sec config
  security_cfg.domain      = security::sec_domain::rrc;
  security_cfg.integ_algo  = security::integrity_algorithm::nia2;
  security_cfg.cipher_algo = security::ciphering_algorithm::nea0;
  security_cfg.k_enc       = k_enc;
  security_cfg.k_int       = k_int;
}
