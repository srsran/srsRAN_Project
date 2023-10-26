/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  fmt::print("algos selected={}\n", context.sec_context.sel_algos.algos_selected);
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
