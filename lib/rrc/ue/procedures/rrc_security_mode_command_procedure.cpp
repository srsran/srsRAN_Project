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
    const byte_buffer&                          du_to_cu_container_,
    rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
    rrc_ue_event_manager&                       event_mng_,
    srslog::basic_logger&                       logger_) :
  context(context_),
  du_to_cu_container(du_to_cu_container_),
  rrc_ue(rrc_ue_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
}

void rrc_security_mode_command_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // create new transaction for RRCSecurityModeCommand
  transaction = event_mng.transactions.create_transaction(rrc_smc_timeout_ms);

  // send RRC setup to UE
  send_rrc_security_mode_command();

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

  CORO_RETURN();
}

void rrc_security_mode_command_procedure::send_rrc_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  // security_mode_cmd_s& rrc_smc = dl_dcch_msg.msg.c1().security_mode_cmd();
  // fill_asn1_rrc_smc_msg(rrc_smc, du_to_cu_container, transaction.id());
  rrc_ue.on_new_dl_dcch(dl_dcch_msg);
}

