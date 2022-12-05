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

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("rnti=0x{:x}: \"{}\" initialized.", context.c_rnti, name());
  // create new transaction for RRCSecurityModeCommand
  transaction = event_mng.transactions.create_transaction(rrc_smc_timeout_ms);

  // select security algorithms to be used
  if (not select_security_algo()) {
    logger.debug("{}: \"{}\" could not select security algorithms.", context.c_rnti, name());
    rrc_ue.on_ue_delete_request(); // delete UE context if SMC fails
  } else {
    // send RRC SMC to UE
    logger.debug("rnti=0x{:x}: \"{}\" selected security algorithms. Integrity=NIA{}, Ciphering=NEA{}",
                 context.c_rnti,
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
      logger.debug("{}: \"{}\" finished successfully.", context.c_rnti, name());
      procedure_result = true;
    } else {
      logger.debug("{}: \"{}\" timed out.", context.c_rnti, name());
      rrc_ue.on_ue_delete_request(); // delete UE context if SMC fails
    }
  }
  logger.debug("rnti=0x{:x}: \"{}\" finalized.", context.c_rnti, name());
  CORO_RETURN(procedure_result);
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
      int_algo_found     = true;
      sec_cfg.integ_algo = security::integrity_algorithm_from_number(algo_id);
      break;
    }
  }
  logger.debug("0x{:x}: \"{}\" selected integrity algorithm NIA{}. ", context.c_rnti, name(), sec_cfg.integ_algo);

  // Select preferred ciphering algorithm.
  std::array<security::ciphering_algorithm, n_algos_ciph> ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                                 security::ciphering_algorithm::nea2,
                                                                                 security::ciphering_algorithm::nea1,
                                                                                 security::ciphering_algorithm::nea3};
  bool                                                    ciph_algo_found     = false;
  for (unsigned i = 0; i < n_algos_ciph; ++i) {
    uint16_t algo_id = to_number(ciph_algo_pref_list[i]);
    if (algo_id == 0) {
      ciph_algo_found     = true;
      sec_cfg.cipher_algo = security::ciphering_algorithm::nea0;
      break;
    }
    if (sec_ctx.supported_int_algos[algo_id - 1]) {
      ciph_algo_found     = true;
      sec_cfg.cipher_algo = security::ciphering_algorithm_from_number(algo_id - 1);
      break;
    }
  }
  logger.debug("0x{:x}: \"{}\" selected ciphering algorithm NEA{}. ", context.c_rnti, name(), sec_cfg.cipher_algo);
  return !(not int_algo_found || not ciph_algo_found);
}

void rrc_security_mode_command_procedure::generate_as_keys()
{
  // TODO store these somewhere else
  security::sec_as_key k_rrc_int;
  security::sec_as_key k_rrc_enc;
  security::sec_as_key k_up_int;
  security::sec_as_key k_up_enc;

  // Generate K_rrc_enc and K_rrc_int
  security::generate_k_rrc(sec_cfg.k_rrc_enc, sec_cfg.k_rrc_int, sec_ctx.k, sec_cfg.cipher_algo, sec_cfg.integ_algo);

  // Generate K_up_enc and K_up_int
  security::generate_k_up(sec_cfg.k_up_enc, sec_cfg.k_up_int, sec_ctx.k, sec_cfg.cipher_algo, sec_cfg.integ_algo);

  logger.info(sec_ctx.k.data(), 32, "K_gNB (k_gnb)");
  logger.info(k_rrc_int.data(), 32, "RRC Integrity Key (k_rrc_int)");
  logger.info(k_rrc_enc.data(), 32, "RRC Encryption Key (k_rrc_enc)");
  logger.info(k_up_int.data(), 32, "UP Encryption Key (k_up_enc)");
  logger.info(k_up_enc.data(), 32, "UP Integrity Key (k_up_int)");
}

void rrc_security_mode_command_procedure::send_rrc_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  security_mode_cmd_s& rrc_smc = dl_dcch_msg.msg.c1().security_mode_cmd();
  fill_asn1_rrc_smc_msg(rrc_smc, sec_cfg.integ_algo, sec_cfg.cipher_algo, transaction.id());
  rrc_ue.on_new_dl_dcch(dl_dcch_msg);
}
