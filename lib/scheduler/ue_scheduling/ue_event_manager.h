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

#include "../config/sched_config_manager.h"
#include "../slicing/inter_slice_scheduler.h"
#include "../ue_context/ue.h"
#include "ue_fallback_scheduler.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ran/du_types.h"

namespace srsran {

class cell_metrics_handler;
class scheduler_event_logger;
class uci_scheduler_impl;
class cell_harq_manager;
class srs_scheduler;
class pdu_indication_pool;

struct cell_creation_event {
  cell_resource_allocator& cell_res_grid;
  cell_harq_manager&       cell_harqs;
  ue_fallback_scheduler&   fallback_sched;
  uci_scheduler_impl&      uci_sched;
  inter_slice_scheduler&   slice_sched;
  srs_scheduler&           srs_sched;
  cell_metrics_handler&    metrics;
  scheduler_event_logger&  ev_logger;
};

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager final : public sched_ue_configuration_handler,
                               public scheduler_feedback_handler,
                               public scheduler_dl_buffer_state_indication_handler,
                               public scheduler_positioning_handler
{
public:
  ue_event_manager(ue_repository& ue_db);
  ~ue_event_manager() override;

  void add_cell(const cell_creation_event& cell_ev);
  void rem_cell(du_cell_index_t cell_index);

  /// UE Add/Mod/Remove interface.
  void handle_ue_creation(ue_config_update_event ev) override;
  void handle_ue_reconfiguration(ue_config_update_event ev) override;
  void handle_ue_deletion(ue_config_delete_event ev) override;
  void handle_ue_config_applied(du_ue_index_t ue_idx) override;

  /// Scheduler feedback handler interface.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_crc_indication(const ul_crc_indication& crc) override;
  void handle_uci_indication(const uci_indication& uci) override;
  void handle_srs_indication(const srs_indication& srs) override;
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;

  /// Scheduler DL buffer state indication handler interface.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override;

  /// Positioning interface.
  void handle_positioning_measurement_request(const positioning_measurement_request& req) override;
  void handle_positioning_measurement_stop(du_cell_index_t cell_index, rnti_t pos_rnti) override;

  void
  handle_error_indication(slot_point sl_tx, du_cell_index_t cell_index, scheduler_slot_handler::error_outcome event);

  /// Process events for a given slot and cell index.
  void run(slot_point sl, du_cell_index_t cell_index);

private:
  class ue_dl_buffer_occupancy_manager;

  struct common_event_t {
    using callback_type = unique_function<void(), 64, true>;

    du_ue_index_t ue_index = MAX_NOF_DU_UES;
    callback_type callback;

    template <typename Callable>
    common_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {
    }
  };
  struct cell_event_t {
    using callback_type = unique_function<void(ue_cell&), 32, true>;

    du_ue_index_t ue_index = MAX_NOF_DU_UES;
    callback_type callback;
    const char*   event_name;
    bool          warn_if_ignored;

    template <typename Callable>
    cell_event_t(du_ue_index_t ue_index_, Callable&& c, const char* event_name_, bool log_warn_if_event_ignored) :
      ue_index(ue_index_),
      callback(std::forward<Callable>(c)),
      event_name(event_name_),
      warn_if_ignored(log_warn_if_event_ignored)
    {
    }
  };

  using common_event_queue = concurrent_queue<common_event_t,
                                              concurrent_queue_policy::lockfree_mpmc,
                                              concurrent_queue_wait_policy::non_blocking>;
  using cell_event_queue   = concurrent_queue<cell_event_t,
                                              concurrent_queue_policy::lockfree_mpmc,
                                              concurrent_queue_wait_policy::non_blocking>;

  void process_common(slot_point sl, du_cell_index_t cell_index);
  void process_cell_specific(du_cell_index_t cell_index);
  bool cell_exists(du_cell_index_t cell_index) const;

  void
  log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name = "Event", bool warn_if_ignored = true) const;
  void log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const;

  void handle_harq_ind(ue_cell&                               ue_cc,
                       slot_point                             uci_sl,
                       span<const mac_harq_ack_report_status> harq_bits,
                       std::optional<float>                   pucch_snr);
  void handle_csi(ue_cell& ue_cc, const csi_report_data& csi_rep);

  /// List of added and configured cells.
  struct du_cell {
    const cell_configuration* cfg            = nullptr;
    cell_resource_allocator*  res_grid       = nullptr;
    cell_harq_manager*        cell_harqs     = nullptr;
    ue_fallback_scheduler*    fallback_sched = nullptr;
    uci_scheduler_impl*       uci_sched      = nullptr;
    inter_slice_scheduler*    slice_sched    = nullptr;
    srs_scheduler*            srs_sched      = nullptr;
    cell_metrics_handler*     metrics        = nullptr;
    scheduler_event_logger*   ev_logger      = nullptr;
  };

  ue_repository&        ue_db;
  srslog::basic_logger& logger;

  std::unique_ptr<pdu_indication_pool> ind_pdu_pool;

  std::array<du_cell, MAX_NOF_DU_CELLS> du_cells{};

  /// Pending Events list per cell.
  static_vector<cell_event_queue, MAX_NOF_DU_CELLS> cell_specific_events;

  /// Pending Events list common to all cells. We use this list for events that require synchronization across
  /// UE carriers when CA is enabled (e.g. SR, BSR, reconfig).
  common_event_queue common_events;

  slot_point last_sl;

  std::unique_ptr<ue_dl_buffer_occupancy_manager> dl_bo_mng;
};

} // namespace srsran
