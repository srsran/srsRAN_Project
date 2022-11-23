/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_security_mode_command_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
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

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("rnti=0x{:x}: \"{}\" initialized.", context.c_rnti, name());
  // create new transaction for RRCSecurityModeCommand
  transaction = event_mng.transactions.create_transaction(rrc_smc_timeout_ms);

  // select security algorithms to be used
  if (not select_security_algo()) {
    logger.debug("{}: \"{}\" could not select security algorithms.", context.c_rnti, name());
    rrc_ue.on_ue_delete_request();
  } else {
    // send RRC setup to UE
    send_rrc_security_mode_command();
    logger.debug("rnti=0x{:x}: \"{}\" selected security algorithms. Integrity=NIA{}, Ciphering=NEA{}",
                 context.c_rnti,
                 name(),
                 int_algo,
                 ciph_algo);

    // Await UE response
    CORO_AWAIT(transaction);

    auto coro_res = transaction.result();
    if (coro_res.has_value()) {
      logger.debug("{}: \"{}\" finished successfully.", context.c_rnti, name());
      // send_initial_ue_msg(coro_res.value());
    } else {
      logger.debug("{}: \"{}\" timed out.", context.c_rnti, name());
      // rrc_ue.on_ue_delete_request();
    }
  }
  CORO_RETURN();
}

bool rrc_security_mode_command_procedure::select_security_algo()
{
  constexpr unsigned n_algos_int  = 3; // NIA 1...3
  constexpr unsigned n_algos_ciph = 4; // NEA 0...3

  // Select preferred integrity algorithm.
  std::array<security::integrity_algorithm, n_algos_int> inc_algo_pref_list = {
      security::integrity_algorithm::nia2, security::integrity_algorithm::nia1, security::integrity_algorithm::nia3};
  bool int_algo_found = false;
  for (unsigned i = 0; i < n_algos_int; ++i) {
    uint16_t algo_id = to_number(inc_algo_pref_list[i]);
    if (sec_ctx.supported_int_algos[algo_id]) {
      int_algo_found = true;
      int_algo       = security::integrity_algorithm_from_number(algo_id);
      break;
    }
  }
  logger.debug("0x{:x}: \"{}\" selected integrity algorithm NIA{}. ", context.c_rnti, name(), int_algo);

  // Select preferred ciphering algorithm.
  std::array<security::ciphering_algorithm, n_algos_ciph> ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                                 security::ciphering_algorithm::nea2,
                                                                                 security::ciphering_algorithm::nea1,
                                                                                 security::ciphering_algorithm::nea3};
  bool                                                    ciph_algo_found     = false;
  for (unsigned i = 0; i < n_algos_ciph; ++i) {
    uint16_t algo_id = to_number(ciph_algo_pref_list[i]);
    if (algo_id == 0) {
      ciph_algo_found = true;
      ciph_algo       = security::ciphering_algorithm::nea0;
      break;
    }
    if (sec_ctx.supported_int_algos[algo_id - 1]) {
      ciph_algo_found = true;
      ciph_algo       = security::ciphering_algorithm_from_number(algo_id - 1);
      break;
    }
  }
  logger.debug("0x{:x}: \"{}\" selected ciphering algorithm NEA{}. ", context.c_rnti, name(), ciph_algo);
  return !(not int_algo_found || not ciph_algo_found);
}

void rrc_security_mode_command_procedure::send_rrc_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  security_mode_cmd_s& rrc_smc = dl_dcch_msg.msg.c1().security_mode_cmd();
  fill_asn1_rrc_smc_msg(rrc_smc, int_algo, ciph_algo, transaction.id());
  rrc_ue.on_new_dl_dcch(dl_dcch_msg);
}

