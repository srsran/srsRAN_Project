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

#include "rrc_reestablishment_procedure.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(rrc_ue_context_t&                     context_,
                                                             const ue_index_t                      old_ue_index_,
                                                             rrc_ue_reestablishment_proc_notifier& rrc_ue_notifier_,
                                                             rrc_ue_du_processor_notifier& du_processor_notifier_,
                                                             rrc_ue_event_manager&         event_mng_,
                                                             srslog::basic_logger&         logger_) :
  context(context_),
  old_ue_index(old_ue_index_),
  rrc_ue(rrc_ue_notifier_),
  du_processor_notifier(du_processor_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // create SRB1
  create_srb1();

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

void rrc_reestablishment_procedure::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = old_ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = srb1_pdcp_cfg;
  du_processor_notifier.on_create_srb(srb1_msg);

  // activate SRB1 PDCP security
  rrc_ue.on_new_as_security_context();
}

void rrc_reestablishment_procedure::send_rrc_reestablishment()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_reest();
  rrc_reest_s& rrc_reest       = dl_dcch_msg.msg.c1().rrc_reest();
  rrc_reest.rrc_transaction_id = transaction.id();
  rrc_reest.crit_exts.set_rrc_reest();

  // if the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
  // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7
  if (get_du_index_from_ue_index(old_ue_index) == get_du_index_from_ue_index(context.ue_index)) {
    rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg, old_ue_index);
  } else {
    rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}
