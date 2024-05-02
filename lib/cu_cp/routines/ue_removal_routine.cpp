/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_removal_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

ue_removal_routine::ue_removal_routine(ue_index_t                       ue_index_,
                                       cu_cp_rrc_ue_notifier&           rrc_du_notifier_,
                                       cu_cp_e1ap_ue_removal_notifier*  e1ap_notifier_,
                                       cu_cp_f1ap_ue_removal_notifier&  f1ap_notifier_,
                                       ngap_ue_context_removal_handler& ngap_removal_handler_,
                                       ue_manager&                      ue_mng_,
                                       srslog::basic_logger&            logger_) :
  ue_index(ue_index_),
  rrc_du_notifier(rrc_du_notifier_),
  e1ap_notifier(e1ap_notifier_),
  f1ap_notifier(f1ap_notifier_),
  ngap_removal_handler(ngap_removal_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
}

void ue_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized", ue_index, name());

  // Remove RRC UE
  rrc_du_notifier.remove_ue(ue_index);

  // Remove Bearer Context from E1AP
  if (e1ap_notifier != nullptr) {
    e1ap_notifier->remove_ue(ue_index);
  }

  // Remove UE Context from F1AP
  f1ap_notifier.remove_ue(ue_index);

  // Remove UE Context from NGAP
  ngap_removal_handler.remove_ue_context(ue_index);

  // Remove UE from UE manager
  ue_mng.remove_ue(ue_index);

  logger.info("ue={}: \"{}\" finalized", ue_index, name());

  CORO_RETURN();
}
