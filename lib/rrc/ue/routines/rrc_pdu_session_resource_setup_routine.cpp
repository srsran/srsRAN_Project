/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_pdu_session_resource_setup_routine.h"
#include "../procedures/rrc_reconfiguration_procedure.h"
#include "../rrc_asn1_helpers.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_pdu_session_resource_setup_routine::rrc_pdu_session_resource_setup_routine(
    rrc_ue_context_t&                     context_,
    rrc_ue_reconfiguration_proc_notifier& rrc_ue_notifier_,
    rrc_ue_event_manager&                 event_mng_,
    srslog::basic_logger&                 logger_) :
  context(context_), rrc_ue_notifier(rrc_ue_notifier_), event_mng(event_mng_), logger(logger_)
{
  // Allocate DRB, depending on QoSFlowSetupRequestList, more than one DRB could be needed
  drb_to_add = allocate_new_drb(/* qos params*/);
}

void rrc_pdu_session_resource_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("rnti=0x{:x}: \"{}\" initialized.", context.c_rnti, name());

  // TODO: Retrieve PDCP and SDAP configs

  {
    // prepare BearerContextSetupRequest and call E1 notifier
    e1ap_bearer_context_setup_request_message request;

    // CORO_AWAIT(e1_notifier->pdu_session_resource_setup_request(context.c_rnti, new_drb.drb_id, new_drb.pdcp_config,
    // new_drb.sdap_config));

    // Wait for BearerContextSetupResponse
  }

  // Register required DRB resources at DU
  {
    // prepare UE Context Modification Request and call F1 notifier
    f1ap_ue_context_modification_request_message request;

    // Wait for UE Context Modification Response

    // store cellGroupConfig, TEID, etc in UE context
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
    // prepare BearerContextModificationRequest and call E1 notifier
    e1ap_bearer_context_modification_request_message request;

    // Wait for BearerContextModificationResponse
  }

  {
    // prepare RRC Reconfiguration
    // TODO: set field in reconfig_args accordingly

    // Trigger reconfig procedure
    CORO_AWAIT_VALUE(
        procedure_result,
        launch_async<rrc_reconfiguration_procedure>(context, reconfig_args, rrc_ue_notifier, event_mng, logger));
  }

  logger.debug("rnti=0x{:x}: \"{}\" finalized.", context.c_rnti, name());
  CORO_RETURN(procedure_result);
}

drb_context rrc_pdu_session_resource_setup_routine::allocate_new_drb()
{
  if (context.drbs.size() >= MAX_NOF_DRBS) {
    logger.error("rnti=0x{:x}: \"{}\" failed. No more DRBs available.", context.c_rnti, name());
    return {};
  }

  drb_id_t new_drb_id = drb_id_t::drb1;
  for (const auto& drbs : context.drbs) {
    if (drbs.drb_id == new_drb_id) {
      /// try next
      new_drb_id = uint_to_drb_id(drb_id_to_uint(new_drb_id) + 1);
      if (new_drb_id == drb_id_t::invalid) {
        logger.error("rnti=0x{:x}: \"{}\" failed. No more DRBs available.", context.c_rnti, name());
        return {};
      }
    } else {
      // found a free DRB
      break;
    }
  }

  drb_context new_drb;
  new_drb.drb_id = new_drb_id;
  context.drbs.push_back(new_drb);

  return new_drb;
}