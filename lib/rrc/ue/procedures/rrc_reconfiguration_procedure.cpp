/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_reconfiguration_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_reconfiguration_procedure::rrc_reconfiguration_procedure(rrc_ue_context_t& context_,
                                                             const cu_cp_rrc_reconfiguration_procedure_request& args_,
                                                             rrc_ue_reconfiguration_proc_notifier& rrc_ue_notifier_,
                                                             rrc_ue_event_manager&                 event_mng_,
                                                             srslog::basic_logger&                 logger_) :
  context(context_), args(args_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
}

void rrc_reconfiguration_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (context.state != rrc_state::connected) {
    logger.error("ue={} \"{}\" failed - UE is not RRC connected", context.ue_index, name());
    CORO_EARLY_RETURN(false);
  }

  logger.debug("ue={} \"{}\" initialized", context.ue_index, name());
  // create new transaction for RRC Reconfiguration procedure
  transaction = event_mng.transactions.create_transaction(timeout_ms);

  send_rrc_reconfiguration();
  CORO_AWAIT(transaction);

  auto coro_res = transaction.result();
  if (coro_res.has_value()) {
    logger.debug("ue={} \"{}\" finished successfull.", context.ue_index, name());
    procedure_result = true;
  } else {
    logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
    rrc_ue.on_ue_delete_request(); // delete UE context if reconfig fails
  }

  logger.debug("ue={} \"{}\" finalized.", context.ue_index, name());
  CORO_RETURN(procedure_result);
}

void rrc_reconfiguration_procedure::send_rrc_reconfiguration()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_recfg();
  rrc_recfg_s& rrc_reconfig = dl_dcch_msg.msg.c1().rrc_recfg();
  fill_asn1_rrc_reconfiguration_msg(rrc_reconfig, transaction.id(), args);
  rrc_ue.on_new_dl_dcch(dl_dcch_msg);
}