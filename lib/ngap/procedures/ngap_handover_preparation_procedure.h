/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "procedures/ngap_transaction_manager.h"
#include "ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_cp {

async_task<ngap_handover_preparation_response>
start_ngap_handover_preparation(const ngap_handover_preparation_request& req,
                                const plmn_identity&                     serving_plmn,
                                const ngap_ue_ids&                       ue_ids,
                                ngap_message_notifier&                   amf_notifier,
                                ngap_rrc_ue_notifier&                    rrc_ue_notifier,
                                ngap_cu_cp_notifier&                     cu_cp_notifier,
                                ngap_transaction_manager&                ev_mng,
                                timer_factory                            timers,
                                ngap_ue_logger&                          logger);

} // namespace srsran::srs_cu_cp
