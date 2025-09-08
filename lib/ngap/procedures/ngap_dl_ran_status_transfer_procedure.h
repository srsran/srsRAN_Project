/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap_transaction_manager.h"
#include "ue_context/ngap_ue_logger.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_cp {

async_task<expected<ngap_dl_ran_status_transfer>>
start_ngap_dl_status_transfer_procedure(ue_index_t                ue_index,
                                        ngap_transaction_manager& ev_mng,
                                        timer_factory             timers,
                                        ngap_ue_logger&           logger);

} // namespace srsran::srs_cu_cp
