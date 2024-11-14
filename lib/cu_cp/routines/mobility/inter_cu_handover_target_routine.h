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

#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/ngap/ngap_handover.h"

namespace srsran::srs_cu_cp {

async_task<ngap_handover_resource_allocation_response>
start_inter_cu_handover_target_routine(const ngap_handover_request& request,
                                       e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng,
                                       f1ap_ue_context_manager&     f1ap_ue_ctxt_mng,
                                       cu_cp_ue_removal_handler&    ue_removal_handler,
                                       ue_manager&                  ue_mng,
                                       const security_indication_t& default_security_indication,
                                       srslog::basic_logger&        logger);

} // namespace srsran::srs_cu_cp
