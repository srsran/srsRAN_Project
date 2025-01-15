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
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief Mobility manager configuration.
struct mobility_manager_cfg {
  bool trigger_handover_from_measurements = false; ///< Set to true to trigger HO when neighbor becomes stronger.
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
