/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_processor_routine_manager_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

du_processor_routine_manager_test::du_processor_routine_manager_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create routine manager
  routine_mng = std::make_unique<du_processor_routine_manager>(
      e1ap_ctrl_notifier, f1ap_ue_ctxt_notifier, rrc_du_notifier, ue_mng, cu_cp_logger);
}

du_processor_routine_manager_test::~du_processor_routine_manager_test()
{
  // flush logger after each test
  srslog::flush();
}

void du_processor_routine_manager_test::init_security_config()
{
  const char* k_rrc_enc_cstr = "4ea96992c8c7e82977231ad001309062ae9f31ead90a4d0842af6cd25cb44dc4";
  const char* k_rrc_int_cstr = "aeeb5e0ae02c6188ecb1625c4a9e022fdfc2a1fc845b44b44443ac9a3bda667c";

  // Pack hex strings into srsgnb types
  security::sec_as_key k_rrc_enc = make_sec_as_key(k_rrc_enc_cstr);
  security::sec_as_key k_rrc_int = make_sec_as_key(k_rrc_int_cstr);

  // Create expected SRB1 sec config
  security_cfg.integ_algo  = security::integrity_algorithm::nia2;
  security_cfg.cipher_algo = security::ciphering_algorithm::nea0;
  security_cfg.k_rrc_enc   = k_rrc_enc;
  security_cfg.k_rrc_int   = k_rrc_int;
}
