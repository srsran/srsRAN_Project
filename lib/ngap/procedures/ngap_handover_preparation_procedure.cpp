/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "ngap_handover_preparation_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

async_task<ngap_handover_preparation_response>
srsran::srs_cu_cp::start_ngap_handover_preparation(const ngap_handover_preparation_request& req,
                                                   const plmn_identity&                     serving_plmn,
                                                   const ngap_ue_ids&                       ue_ids,
                                                   ngap_message_notifier&                   amf_notifier,
                                                   ngap_rrc_ue_notifier&                    rrc_ue_notifier,
                                                   ngap_cu_cp_notifier&                     cu_cp_notifier,
                                                   ngap_transaction_manager&                ev_mng,
                                                   timer_factory                            timers,
                                                   ngap_ue_logger&                          logger)
{
  logger.log_error("NG Handover failed. Cause: NG handover not supported.");
  auto err_function = [](coro_context<async_task<ngap_handover_preparation_response>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(ngap_handover_preparation_response{false});
  };
  return launch_async(std::move(err_function));
}
