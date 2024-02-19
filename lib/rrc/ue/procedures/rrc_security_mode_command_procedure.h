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

#pragma once

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of AS security keys in the RRC UE.
/// TODO Add seqdiag
class rrc_security_mode_command_procedure
{
public:
  rrc_security_mode_command_procedure(rrc_ue_context_t&                           context_,
                                      security::security_context                  sec_ctx,
                                      rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                                      rrc_ue_event_manager&                       ev_mng_,
                                      rrc_ue_logger&                              logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Security Mode Command Procedure"; }

private:
  /// \remark Select security algorithms based on the UE capabilities
  /// provided by the NGAP.
  bool select_security_algo();

  /// \remark Generate AS keys based on selected algorithms
  /// and provided K_gNB.
  void generate_as_keys();

  /// \remark Send RRC Security Mode Command, see section 5.3.3 in TS 38.331
  void send_rrc_security_mode_command();

  rrc_ue_context_t& context;

  rrc_ue_security_mode_command_proc_notifier& rrc_ue;    // handler to the parent RRC UE object
  rrc_ue_event_manager&                       event_mng; // event manager for the RRC UE entity
  rrc_ue_logger&                              logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  bool procedure_result = false;
};

/// \brief Fills ASN.1 RRC Security Mode Command struct.
/// \param[out] rrc_smc The RRC security mode command ASN.1 struct to fill.
/// \param[in] int_algo The selected integrity protection algorithm.
/// \param[in] ciph_algo The selected ciphering algorithm.
/// \param[in] rrc_transaction_id The RRC transaction id.
inline void fill_asn1_rrc_smc_msg(asn1::rrc_nr::security_mode_cmd_s&   rrc_smc,
                                  const security::integrity_algorithm& int_algo,
                                  const security::ciphering_algorithm& ciph_algo,
                                  uint8_t                              rrc_transaction_id)
{
  using namespace asn1::rrc_nr;
  security_mode_cmd_ies_s& smc_ies = rrc_smc.crit_exts.set_security_mode_cmd();
  rrc_smc.rrc_transaction_id       = rrc_transaction_id;

  // Set security algorithms
  security_cfg_smc_s&       security_cfg_smc       = smc_ies.security_cfg_smc;
  security_algorithm_cfg_s& security_algorithm_cfg = security_cfg_smc.security_algorithm_cfg;

  security_algorithm_cfg.integrity_prot_algorithm_present = true;
  switch (int_algo) {
    case security::integrity_algorithm::nia0:
      security_algorithm_cfg.integrity_prot_algorithm = integrity_prot_algorithm_e::nia0;
      break;
    case security::integrity_algorithm::nia1:
      security_algorithm_cfg.integrity_prot_algorithm = integrity_prot_algorithm_e::nia1;
      break;
    case security::integrity_algorithm::nia2:
      security_algorithm_cfg.integrity_prot_algorithm = integrity_prot_algorithm_e::nia2;
      break;
    case security::integrity_algorithm::nia3:
      security_algorithm_cfg.integrity_prot_algorithm = integrity_prot_algorithm_e::nia3;
      break;
  }
  switch (ciph_algo) {
    case security::ciphering_algorithm::nea0:
      security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_e::nea0;
      break;
    case security::ciphering_algorithm::nea1:
      security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_e::nea1;
      break;
    case security::ciphering_algorithm::nea2:
      security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_e::nea2;
      break;
    case security::ciphering_algorithm::nea3:
      security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_e::nea3;
      break;
  }
}

} // namespace srs_cu_cp
} // namespace srsran
