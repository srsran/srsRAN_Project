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
    srslog::basic_logger&                       logger_) :
  context(context_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
}

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} \"{}\" initialized", context.ue_index, name());
  // create new transaction for RRCSecurityModeCommand
  transaction =
      event_mng.transactions.create_transaction(std::chrono::milliseconds(context.cfg.rrc_procedure_timeout_ms));

  // select security algorithms to be used
  if (not select_security_algo()) {
    logger.debug("{}: \"{}\" could not select security algorithms", context.ue_index, name());
  } else {
    // send RRC SMC to UE
    logger.debug("ue={} \"{}\" selected security algorithms integrity=NIA{} ciphering=NEA{}",
                 context.ue_index,
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
      logger.debug("ue={} \"{}\" finished successfully", context.ue_index, name());
      procedure_result = true;
    } else {
      logger.warning(
          "ue={} \"{}\" timed out after {}ms", context.ue_index, name(), context.cfg.rrc_procedure_timeout_ms);
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
  logger.debug("Integrity protection algorithms supported list: {}", context.sec_context.supported_int_algos);
  logger.debug("Ciphering algorithms preference list: {}", context.sec_context.supported_enc_algos);
  if (not context.sec_context.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list)) {
    logger.error("ue={} \"{}\" could not select security algorithm", context.ue_index, name());
    return false;
  }
  logger.debug("ue={} \"{}\" selected security algorithms NIA=NIA{} NEA=NEA{} ",
               context.ue_index,
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
