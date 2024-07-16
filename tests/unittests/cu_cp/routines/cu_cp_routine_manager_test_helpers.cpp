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

#include "cu_cp_routine_manager_test_helpers.h"
#include "tests/unittests/rrc/rrc_ue_test_messages.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

static srsran::security::sec_as_config init_security_config()
{
  const char* k_enc_cstr = "4ea96992c8c7e82977231ad001309062ae9f31ead90a4d0842af6cd25cb44dc4";
  const char* k_int_cstr = "aeeb5e0ae02c6188ecb1625c4a9e022fdfc2a1fc845b44b44443ac9a3bda667c";

  // Pack hex strings into srsgnb types
  security::sec_key k_enc = make_sec_key(k_enc_cstr);
  security::sec_key k_int = make_sec_key(k_int_cstr);

  // Create expected SRB1 sec config
  srsran::security::sec_as_config security_cfg;
  security_cfg.domain      = security::sec_domain::rrc;
  security_cfg.integ_algo  = security::integrity_algorithm::nia2;
  security_cfg.cipher_algo = security::ciphering_algorithm::nea0;
  security_cfg.k_enc       = k_enc;
  security_cfg.k_int       = k_int;
  return security_cfg;
}

cu_cp_routine_manager_test::cu_cp_routine_manager_test() :
  security_cfg(init_security_config()), cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    cucfg.ue.inactivity_timer     = std::chrono::seconds{10};
    pdcp_config pdcp_cfg{
        pdcp_rb_type::drb, pdcp_rlc_mode::am, {}, {}, {pdcp_sn_size::size12bits}, {pdcp_sn_size::size12bits}};
    cucfg.bearers.drb_config = {{uint_to_five_qi(9), {pdcp_cfg}}, {uint_to_five_qi(7), {pdcp_cfg}}};
    return cucfg;
  }())
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create routine manager
  routine_mng =
      std::make_unique<cu_cp_routine_manager>(ue_mng, cu_cp_cfg.security.default_security_indication, cu_cp_logger);
}

cu_cp_routine_manager_test::~cu_cp_routine_manager_test()
{
  // flush logger after each test
  srslog::flush();
}
