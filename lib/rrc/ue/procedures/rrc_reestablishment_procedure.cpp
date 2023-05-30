/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_reestablishment_procedure.h"
#include "../rrc_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(rrc_ue_context_t&                     context_,
                                                             const ue_index_t                      old_ue_index_,
                                                             rrc_ue_reestablishment_proc_notifier& rrc_ue_notifier_,
                                                             rrc_ue_event_manager&                 event_mng_,
                                                             srslog::basic_logger&                 logger_) :
  context(context_), old_ue_index(old_ue_index_), rrc_ue(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // create new transaction for RRC Reestablishment
  transaction = event_mng.transactions.create_transaction(rrc_reest_timeout_ms);

  // send RRC Reestablishment to UE
  send_rrc_reestablishment();

  // Await UE response
  CORO_AWAIT(transaction);

  auto coro_res = transaction.result();
  if (coro_res.has_value()) {
    logger.debug("ue={} \"{}\" finished successfully", context.ue_index, name());
    context.state = rrc_state::connected;
  } else {
    logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
    rrc_ue.on_ue_delete_request(cause_t::protocol);
  }

  CORO_RETURN();
}

void rrc_reestablishment_procedure::send_rrc_reestablishment()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_reest();
  rrc_reest_s& rrc_reest       = dl_dcch_msg.msg.c1().rrc_reest();
  rrc_reest.rrc_transaction_id = transaction.id();
  rrc_reest.crit_exts.set_rrc_reest();

  rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg, old_ue_index);
}
