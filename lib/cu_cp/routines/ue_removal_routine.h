/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../adapters/cu_cp_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../cell_meas_manager/cell_meas_manager_impl.h"
#include "../cu_cp_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "../ue_manager/ue_task_scheduler.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Removes a UE from the CU-CP
class ue_removal_routine
{
public:
  ue_removal_routine(ue_index_t                      ue_index_,
                     cu_cp_rrc_ue_removal_notifier&  rrc_du_notifier_,
                     cu_cp_e1ap_ue_removal_notifier* e1ap_notifier_,
                     cu_cp_f1ap_ue_removal_notifier& f1ap_notifier_,
                     cu_cp_ngap_control_notifier&    ngap_notifier_,
                     cell_meas_manager&              cell_meas_mng_,
                     ue_manager&                     ue_mng_,
                     srslog::basic_logger&           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Removal Routine"; }

private:
  const ue_index_t                ue_index;
  cu_cp_rrc_ue_removal_notifier&  rrc_du_notifier; // to trigger removal of the UE at the RRC
  cu_cp_e1ap_ue_removal_notifier* e1ap_notifier;   // to trigger removal of the UE at the E1AP
  cu_cp_f1ap_ue_removal_notifier& f1ap_notifier;   // to trigger removal of the UE at the F1AP
  cu_cp_ngap_control_notifier&    ngap_notifier;   // to trigger removal of the UE at the NGAP
  cell_meas_manager&              cell_meas_mng;   // to remove UE context from cell measurement manager
  ue_manager&                     ue_mng;          // to remove UE context from DU processor
  srslog::basic_logger&           logger;
};

} // namespace srs_cu_cp
} // namespace srsran
