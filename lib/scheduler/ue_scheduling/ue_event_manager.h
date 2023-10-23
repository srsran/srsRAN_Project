/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../policy/scheduler_policy.h"
#include "../support/slot_event_list.h"
#include "ue.h"
#include "ue_srb0_scheduler.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

class scheduler_metrics_handler;
class scheduler_event_logger;

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager final : public scheduler_ue_configurator,
                               public scheduler_feedback_handler,
                               public scheduler_dl_buffer_state_indication_handler
{
public:
  ue_event_manager(const scheduler_ue_expert_config& expert_cfg_,
                   ue_repository&                    ue_db,
                   sched_configuration_notifier&     mac_notifier,
                   scheduler_metrics_handler&        metrics_handler,
                   scheduler_event_logger&           ev_logger);

  void add_cell(const cell_configuration& cell_cfg_, ue_srb0_scheduler& srb0_sched);

  /// UE Add/Mod/Remove interface.
  void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request) override;
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;
  void handle_ue_removal_request(du_ue_index_t ue_index) override;

  /// Scheduler feedback handler interface.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_crc_indication(const ul_crc_indication& crc) override;
  void handle_uci_indication(const uci_indication& uci) override;
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;

  /// Scheduler DL buffer state indication handler interface.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override;

  /// Process events for a given slot and cell index.
  void run(slot_point sl, du_cell_index_t cell_index);

private:
  struct common_event_t {
    du_ue_index_t           ue_index = MAX_NOF_DU_UES;
    unique_function<void()> callback;

    template <typename Callable>
    common_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {
    }
  };
  struct cell_event_t {
    du_ue_index_t                   ue_index = MAX_NOF_DU_UES;
    unique_function<void(ue_cell&)> callback;
    const char*                     event_name;
    bool                            warn_if_ignored;

    template <typename Callable>
    cell_event_t(du_ue_index_t ue_index_, Callable&& c, const char* event_name_, bool log_warn_if_event_ignored) :
      ue_index(ue_index_),
      callback(std::forward<Callable>(c)),
      event_name(event_name_),
      warn_if_ignored(log_warn_if_event_ignored)
    {
    }
  };

  void process_common(slot_point sl, du_cell_index_t cell_index);
  void process_cell_specific(du_cell_index_t cell_index);
  bool cell_exists(du_cell_index_t cell_index) const;

  void
  log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name = "Event", bool warn_if_ignored = true) const;
  void log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const;

  void handle_harq_ind(ue_cell&                               ue_cc,
                       slot_point                             uci_sl,
                       span<const mac_harq_ack_report_status> harq_bits,
                       bool                                   is_pucch_f1,
                       optional<float>                        pucch_snr);
  void handle_csi(ue_cell& ue_cc, const csi_report_data& csi_rep);

  const scheduler_ue_expert_config& expert_cfg;
  ue_repository&                    ue_db;
  sched_configuration_notifier&     mac_notifier;
  scheduler_metrics_handler&        metrics_handler;
  scheduler_event_logger&           ev_logger;
  srslog::basic_logger&             logger;

  /// List of added and configured cells.
  struct du_cell {
    const cell_configuration* cfg = nullptr;

    /// Reference to SRB0 and other bearers scheduler
    ue_srb0_scheduler* srb0_sched = nullptr;
  };
  std::array<du_cell, MAX_NOF_DU_CELLS> du_cells{};

  /// Pending Events list per cell.
  std::array<slot_event_list<cell_event_t>, MAX_NOF_DU_CELLS> cell_specific_events;

  /// Pending Events list common to all cells. We use this list for events that require synchronization across
  /// UE carriers when CA is enabled (e.g. SR, BSR, reconfig).
  slot_event_list<common_event_t> common_events;
  slot_point                      last_sl;
};

} // namespace srsran
