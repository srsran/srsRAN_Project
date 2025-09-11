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

#include "../../cell_meas_manager/cell_meas_manager_impl.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/ngap/ngap_handover.h"

namespace srsran::srs_cu_cp {

async_task<ngap_handover_resource_allocation_response>
start_inter_cu_handover_target_routine(const ngap_handover_request& request,
                                       e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng,
                                       f1ap_ue_context_manager&     f1ap_ue_ctxt_mng,
                                       cu_cp_ue_removal_handler&    ue_removal_handler,
                                       ue_manager&                  ue_mng,
                                       cell_meas_manager&           cell_meas_mngr,
                                       const security_indication_t& default_security_indication,
                                       srslog::basic_logger&        logger);

} // namespace srsran::srs_cu_cp
