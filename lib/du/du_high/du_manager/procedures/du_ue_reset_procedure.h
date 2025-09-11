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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/manual_event.h"
#include <vector>

namespace srsran {
namespace srs_du {

class du_ue_manager;
struct du_manager_params;

/// Task that handles the removal of several UEs in the DU.
class du_ue_reset_procedure
{
public:
  /// Instantiates UE reset procedure.
  /// \param[in] ues_to_reset List of UEs to reset. If empty, all UEs will be reset.
  /// \param[in] ue_mng DU UE manager.
  /// \param[in] du_params DU manager parameters.
  /// \param[in] cause Optional cause for the F1 Reset. If provided, an F1 Reset will be triggered towards the CU.
  /// This is not always desired, e.g. when the reset is triggered by an F1 Reset received from the CU.
  du_ue_reset_procedure(const std::vector<du_ue_index_t>&                  ues_to_reset,
                        du_ue_manager&                                     ue_mng_,
                        const du_manager_params&                           du_params,
                        const std::optional<f1_reset_request::cause_type>& cause = std::nullopt);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> reset_ues();

  std::vector<du_ue_index_t> ues_to_reset;
  du_ue_manager&             ue_mng;
  const du_manager_params&   du_params;
  /// Whether to trigger F1 Reset towards CU after removing UEs.
  const std::optional<f1_reset_request::cause_type> cause;
};
} // namespace srs_du
} // namespace srsran
