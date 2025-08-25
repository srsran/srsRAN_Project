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

#include "procedure_logger.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

class du_ue_manager;
class du_cell_manager;
struct du_manager_params;

/// \brief Procedure used to stop an active DU cell.
/// The UEs associated with the cell being deactivated will be released in the process, either via F1AP UE CONTEXT
/// RELEASE REQUEST, F1 RESET, or silently depending on the configured policy.
class du_cell_stop_procedure
{
public:
  /// Policy used to remove UEs associated with the cell being deactivated.
  enum class ue_removal_mode { trigger_f1_ue_release_request, trigger_f1_reset, no_f1_triggers };

  /// Create DU cell stop procedure.
  /// \param ue_mng DU UE manager.
  /// \param cell_mng DU cell manager.
  /// \param du_params DU manager parameters.
  /// \param cell_index Index of the cell to stop.
  /// \param mode Whether to trigger F1AP UE CONTEXT RELEASE REQUEST or F1 RESET for UEs attached to the cell
  /// being deactivated.
  explicit du_cell_stop_procedure(du_ue_manager&           ue_mng,
                                  du_cell_manager&         cell_mng,
                                  const du_manager_params& du_params,
                                  du_cell_index_t          cell_index,
                                  ue_removal_mode          mode);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> rem_ues_with_matching_pcell();
  async_task<void> await_cu_to_release_ues();
  async_task<void> force_ue_removal();

  du_ue_manager&           ue_mng;
  du_cell_manager&         cell_mng;
  const du_manager_params& du_params;
  const du_cell_index_t    cell_index;
  ue_removal_mode          mode;

  du_procedure_logger proc_logger;

  unique_timer timer;

  std::vector<du_ue_index_t> ues_to_rem;
};

} // namespace srs_du
} // namespace srsran
