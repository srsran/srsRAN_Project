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

#include "rrc_security_mode_command_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_security_mode_command_procedure::rrc_security_mode_command_procedure(
    rrc_ue_context_t&                           context_,
    rrc_init_security_context                   sec_ctx_,
    rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
    rrc_ue_event_manager&                       event_mng_,
    srslog::basic_logger&                       logger_) :
  context(context_), sec_ctx(sec_ctx_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
}

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} \"{}\" initialized", context.ue_index, name());
  // create new transaction for RRCSecurityModeCommand
  transaction = event_mng.transactions.create_transaction(rrc_smc_timeout_ms);

  // select security algorithms to be used
  if (not select_security_algo()) {
    logger.debug("{}: \"{}\" could not select security algorithms", context.ue_index, name());
    rrc_ue.on_ue_delete_request(); // delete UE context if SMC fails
  } else {
    // send RRC SMC to UE
    logger.debug("ue={} \"{}\" selected security algorithms integrity=NIA{} ciphering=NEA{}",
                 context.ue_index,
                 name(),
                 sec_cfg.integ_algo,
                 sec_cfg.cipher_algo);

    generate_as_keys();

    // activate SRB1 PDCP security
    rrc_ue.on_new_security_config(sec_cfg);

    send_rrc_security_mode_command();
    // Await UE response
    CORO_AWAIT(transaction);

    auto coro_res = transaction.result();
    if (coro_res.has_value()) {
      logger.debug("ue={} \"{}\" finished successfully", context.ue_index, name());
      procedure_result = true;
    } else {
      logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
      rrc_ue.on_ue_delete_request(); // delete UE context if SMC fails
    }
  }
  logger.debug("ue={} \"{}\" finalized.", context.ue_index, name());
  CORO_RETURN(procedure_result);
}

bool rrc_security_mode_command_procedure::select_security_algo()
{
  // Select preferred integrity algorithm.
  security::preferred_integrity_algorithms inc_algo_pref_list  = {security::integrity_algorithm::nia2,
                                                                  security::integrity_algorithm::nia1,
                                                                  security::integrity_algorithm::nia3,
                                                                  security::integrity_algorithm::nia0};
  security::preferred_ciphering_algorithms ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                  security::ciphering_algorithm::nea2,
                                                                  security::ciphering_algorithm::nea1,
                                                                  security::ciphering_algorithm::nea3};
  logger.debug("Integrity protection algorithms preference list: {}", inc_algo_pref_list);
  logger.debug("Ciphering algorithms preference list: {}", ciph_algo_pref_list);
  logger.debug("Integrity protection algorithms supported list: {}", sec_ctx.supported_int_algos);
  logger.debug("Ciphering algorithms preference list: {}", sec_ctx.supported_enc_algos);
  if (not security::select_algorithms(sec_cfg,
                                      inc_algo_pref_list,
                                      ciph_algo_pref_list,
                                      sec_ctx.supported_int_algos,
                                      sec_ctx.supported_enc_algos,
                                      logger)) {
    logger.error("ue={} \"{}\" could not select security algorithm", context.ue_index, name());
    return false;
  }
  logger.debug("ue={} \"{}\" selected security algorithms NIA=NIA{} NEA=NEA{} ",
               context.ue_index,
               name(),
               sec_cfg.integ_algo,
               sec_cfg.cipher_algo);
  return true;
}

void rrc_security_mode_command_procedure::generate_as_keys()
{
  // Generate K_rrc_enc and K_rrc_int
  security::generate_k_rrc(sec_cfg.k_rrc_enc, sec_cfg.k_rrc_int, sec_ctx.k, sec_cfg.cipher_algo, sec_cfg.integ_algo);

  // Generate K_up_enc and K_up_int
  security::generate_k_up(sec_cfg.k_up_enc, sec_cfg.k_up_int, sec_ctx.k, sec_cfg.cipher_algo, sec_cfg.integ_algo);

  logger.debug("K_gNB {}", security::sec_as_key_to_string(sec_ctx.k));
  logger.debug("RRC Integrity Key {}", security::sec_as_key_to_string(sec_cfg.k_rrc_int));
  logger.debug("RRC Encryption Key {}", security::sec_as_key_to_string(sec_cfg.k_rrc_enc));
  logger.debug("UP Encryption Key {}", security::sec_as_key_to_string(sec_cfg.k_up_enc));
  logger.debug("UP Integrity Key {}", security::sec_as_key_to_string(sec_cfg.k_up_int));
}

void rrc_security_mode_command_procedure::send_rrc_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  security_mode_cmd_s& rrc_smc = dl_dcch_msg.msg.c1().security_mode_cmd();
  fill_asn1_rrc_smc_msg(rrc_smc, sec_cfg.integ_algo, sec_cfg.cipher_algo, transaction.id());
  rrc_ue.on_new_dl_dcch(dl_dcch_msg);
}
