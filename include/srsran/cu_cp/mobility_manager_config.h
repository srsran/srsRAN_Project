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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief Mobility manager configuration.
struct mobility_manager_cfg {
  bool trigger_handover_from_measurements = false; ///< Set to true to trigger HO when neighbor becomes stronger.
  bool enable_ngap_metrics                = false; ///< Set to true to enable inter gNB handover metrics collection.
  bool enable_rrc_metrics                 = false; ///< Set to true to enable intra gNB metrics collection.
};

/// Methods used by mobility manager to signal handover events to the CU-CP.
class mobility_manager_cu_cp_notifier
{
public:
  virtual ~mobility_manager_cu_cp_notifier() = default;

  /// \brief Notify the CU-CP about an required intra-CU handover.
  virtual async_task<cu_cp_intra_cu_handover_response>
  on_intra_cu_handover_required(const cu_cp_intra_cu_handover_request& request,
                                du_index_t                             source_du_index,
                                du_index_t                             target_du_index) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
