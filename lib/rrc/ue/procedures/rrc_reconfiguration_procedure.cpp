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

#include "rrc_reconfiguration_procedure.h"
#include "../rrc_asn1_helpers.h"
#include "srsran/cu_cp/du_processor.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_reconfiguration_procedure::rrc_reconfiguration_procedure(rrc_ue_context_t&                            context_,
                                                             const rrc_reconfiguration_procedure_request& args_,
                                                             rrc_ue_reconfiguration_proc_notifier& rrc_ue_notifier_,
                                                             rrc_ue_event_manager&                 event_mng_,
                                                             rrc_ue_du_processor_notifier&         du_processor_,
                                                             srslog::basic_logger&                 logger_) :
  context(context_),
  args(args_),
  rrc_ue(rrc_ue_notifier_),
  event_mng(event_mng_),
  du_processor_notifier(du_processor_),
  logger(logger_)
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

  if (args.radio_bearer_cfg.has_value()) {
    for (const rrc_srb_to_add_mod& srb_to_add_mod : args.radio_bearer_cfg->srb_to_add_mod_list) {
      srb_creation_message srb = {};
      srb.ue_index             = context.ue_index;
      srb.srb_id               = srb_to_add_mod.srb_id;
      du_processor_notifier.on_create_srb(srb);
    }
  }

  send_rrc_reconfiguration();
  CORO_AWAIT(transaction);

  auto coro_res = transaction.result();
  if (coro_res.has_value()) {
    logger.debug("ue={} \"{}\" finished successfull.", context.ue_index, name());
    procedure_result = true;
  } else {
    logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
    rrc_ue.on_ue_delete_request(cause_t::protocol); // delete UE context if reconfig fails
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
  rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}
