/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "../metrics/ngap_metrics_aggregator.h"
#include "ngap_transaction_manager.h"
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
                                ngap_ue_transaction_manager&             ev_mng,
                                timer_factory                            timers,
                                ngap_ue_logger&                          logger);

} // namespace srsran::srs_cu_cp
