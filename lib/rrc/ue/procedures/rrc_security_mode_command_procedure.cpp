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

#include "rrc_security_mode_command_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_security_mode_command_procedure::rrc_security_mode_command_procedure(
    rrc_ue_context_t&                           context_,
    security::security_context                  sec_ctx_,
    rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
    rrc_ue_event_manager&                       event_mng_,
    rrc_ue_logger&                              logger_) :
  context(context_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
}

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());
  // create new transaction for RRCSecurityModeCommand
  transaction =
      event_mng.transactions.create_transaction(std::chrono::milliseconds(context.cfg.rrc_procedure_timeout_ms));

  // select security algorithms to be used
  if (not select_security_algo()) {
    logger.log_debug("\"{}\" could not select security algorithms", name());
  } else {
    // send RRC SMC to UE
    logger.log_debug("\"{}\" selected security algorithms integrity=NIA{} ciphering=NEA{}",
                     name(),
                     context.sec_context.sel_algos.integ_algo,
                     context.sec_context.sel_algos.cipher_algo);

    generate_as_keys();

    // activate SRB1 PDCP security
    rrc_ue.on_new_as_security_context();

    send_rrc_security_mode_command();
    // Await UE response
    CORO_AWAIT(transaction);

    if (transaction.has_response()) {
      logger.log_debug("\"{}\" finished successfully", name());
      rrc_ue.on_security_context_sucessful();
      procedure_result = true;
    } else {
      logger.log_warning("\"{}\" timed out after {}ms", name(), context.cfg.rrc_procedure_timeout_ms);
    }
  }
  logger.log_debug("\"{}\" finalized", name());
  CORO_RETURN(procedure_result);
}

bool rrc_security_mode_command_procedure::select_security_algo()
{
  // Select preferred integrity algorithm.
  security::preferred_integrity_algorithms inc_algo_pref_list  = context.cfg.int_algo_pref_list;
  security::preferred_ciphering_algorithms ciph_algo_pref_list = context.cfg.enc_algo_pref_list;
  logger.log_debug("Integrity protection algorithms preference list: {}", inc_algo_pref_list);
  logger.log_debug("Ciphering algorithms preference list: {}", ciph_algo_pref_list);
  logger.log_debug("Integrity protection algorithms supported list: {}", context.sec_context.supported_int_algos);
  logger.log_debug("Ciphering algorithms preference list: {}", context.sec_context.supported_enc_algos);
  if (not context.sec_context.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list)) {
    logger.log_error("\"{}\" could not select security algorithm", name());
    return false;
  }
  logger.log_debug("\"{}\" selected security algorithms NIA=NIA{} NEA=NEA{} ",
                   name(),
                   context.sec_context.sel_algos.integ_algo,
                   context.sec_context.sel_algos.cipher_algo);
  return true;
}

void rrc_security_mode_command_procedure::generate_as_keys()
{
  // Generate K_rrc_enc and K_rrc_int
  context.sec_context.generate_as_keys();
}

void rrc_security_mode_command_procedure::send_rrc_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  security_mode_cmd_s& rrc_smc = dl_dcch_msg.msg.c1().security_mode_cmd();
  fill_asn1_rrc_smc_msg(
      rrc_smc, context.sec_context.sel_algos.integ_algo, context.sec_context.sel_algos.cipher_algo, transaction.id());
  rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}
