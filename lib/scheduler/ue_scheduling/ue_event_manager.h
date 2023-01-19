/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../policy/scheduler_policy.h"
#include "../support/slot_event_list.h"
#include "ue.h"
#include "ue_srb0_scheduler.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

class event_logger;
class scheduler_metrics_handler;

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager : public scheduler_ue_configurator,
                         public scheduler_feedback_handler,
                         public scheduler_dl_buffer_state_indication_handler
{
public:
  ue_event_manager(const scheduler_ue_expert_config& expert_cfg_,
                   ue_list&                          ue_db,
                   sched_configuration_notifier&     mac_notifier,
                   scheduler_metrics_handler&        metrics_handler);

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

  /// Scheduler DL buffer state indication handler interface.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override;

  /// Process events for a given slot and cell index.
  void run(slot_point sl, du_cell_index_t cell_index);

private:
  struct common_event_t {
    du_ue_index_t                        ue_index = MAX_NOF_DU_UES;
    unique_function<void(event_logger&)> callback;

    template <typename Callable>
    common_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {
    }
  };
  struct cell_event_t {
    du_ue_index_t                                  ue_index = MAX_NOF_DU_UES;
    unique_function<void(ue_cell&, event_logger&)> callback;

    template <typename Callable>
    cell_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {
    }
  };

  void process_common(slot_point sl, du_cell_index_t cell_index);
  void process_cell_specific(du_cell_index_t cell_index);
  bool cell_exists(du_cell_index_t cell_index) const;

  void log_invalid_ue_index(du_ue_index_t ue_index) const;
  void log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const;

  void handle_harq_ind(ue_cell& ue_cc, slot_point uci_sl, span<const bool> harq_bits);
  void handle_harq_ind(ue_cell&                                                   ue_cc,
                       slot_point                                                 uci_sl,
                       const bounded_bitset<uci_constants::MAX_NOF_PAYLOAD_BITS>& harq_bits);

  const scheduler_ue_expert_config& expert_cfg;
  ue_list&                          ue_db;
  sched_configuration_notifier&     mac_notifier;
  scheduler_metrics_handler&        metrics_handler;
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

} // namespace srsgnb
