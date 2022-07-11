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

#include "../support/slot_event_list.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/ran/du_types.h"
#include "ue.h"

namespace srsgnb {

class event_logger;

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager : public scheduler_ue_configurator, public scheduler_feedback_handler
{
public:
  ue_event_manager(ue_list& ue_db_, sched_configuration_notifier& mac_notifier_);

  void add_cell_config(const cell_configuration& cell_cfg_);

  /// UE Add/Mod/Remove interface.
  void handle_add_ue_request(const sched_ue_creation_request_message& ue_request) override;
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;
  void handle_ue_delete_request(du_ue_index_t ue_index) override;

  /// Scheduler feedback handler interface.
  void handle_sr_indication(const sr_indication_message& sr) override;
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;

  /// Process events for a given slot and cell index.
  void run(slot_point sl, du_cell_index_t cell_index);

private:
  struct common_event_t {
    du_ue_index_t                        ue_index = MAX_NOF_DU_UES;
    unique_function<void(event_logger&)> callback;

    template <typename Callable>
    common_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {}
  };
  struct carrier_event_t {
    du_ue_index_t                                           ue_index = MAX_NOF_DU_UES;
    unique_function<void(ue_carrier& ue_cc, event_logger&)> callback;

    template <typename Callable>
    carrier_event_t(du_ue_index_t ue_index_, Callable&& c) : ue_index(ue_index_), callback(std::forward<Callable>(c))
    {}
  };

  void process_common(slot_point sl, du_cell_index_t cell_index);
  void process_cell_specific(du_cell_index_t cell_index);
  bool cell_exists(du_cell_index_t cell_index) const;

  void log_invalid_ue_index(du_ue_index_t ue_index) const;
  void log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const;

  ue_list&                      ue_db;
  sched_configuration_notifier& mac_notifier;
  srslog::basic_logger&         logger;

  /// List of added and configured cells.
  std::array<const cell_configuration*, MAX_NOF_DU_UES> cells{};

  /// Pending Events list per cell.
  std::array<slot_event_list<carrier_event_t>, MAX_NOF_DU_CELLS> cell_specific_events;

  /// Pending Events list common to all cells. We use this list for events that require synchronization across
  /// UE carriers when CA is enabled (e.g. SR, BSR, reconfig).
  slot_event_list<common_event_t> common_events;
  slot_point                      last_sl;
};

} // namespace srsgnb
