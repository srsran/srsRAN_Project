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

class ue_cell_event_manager final : public scheduler_feedback_handler, public scheduler_positioning_handler
{
public:
  ue_cell_event_manager(const cell_creation_event& cell_ev, ue_repository& ue_db, srslog::basic_logger& logger);

  void start() { active.store(true, std::memory_order_release); }
  void stop();

  void process_events(slot_point sl_tx);

  // scheduler_feedback_handler methods.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_crc_indication(const ul_crc_indication& crc) override;
  void handle_uci_indication(const uci_indication& uci) override;
  void handle_srs_indication(const srs_indication& srs) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override;

  // scheduler_positioning_handler methods.
  void handle_positioning_measurement_request(const positioning_measurement_request& req) override;
  void handle_positioning_measurement_stop(du_cell_index_t cell_index, rnti_t pos_rnti) override;

private:
  enum class event_result { processed, invalid_ue, invalid_ue_cc };

  struct common_cell_event_t {
    using callback_type = unique_function<event_result(), 64, true>;

    callback_type callback;
    const char*   ev_name = "invalid";
    /// UE index associated with the event. If INVALID_DU_UE_INDEX, the event is not associated with any UE.
    du_ue_index_t ue_index        = INVALID_DU_UE_INDEX;
    bool          warn_if_ignored = true;

    common_cell_event_t() = default;
    template <typename Callable>
    common_cell_event_t(const char* ev_name_, Callable&& callable, bool warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)), ev_name(ev_name_), warn_if_ignored(warn_if_ignored_)
    {
    }
    template <typename Callable>
    common_cell_event_t(const char*   ev_name_,
                        du_ue_index_t ue_index_,
                        Callable&&    callable,
                        bool          warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)),
      ev_name(ev_name_),
      ue_index(ue_index_),
      warn_if_ignored(warn_if_ignored_)
    {
    }
  };

  using common_cell_event_queue = concurrent_queue<common_cell_event_t,
                                                   concurrent_queue_policy::lockfree_mpmc,
                                                   concurrent_queue_wait_policy::non_blocking>;

  void push_cell_event(du_cell_index_t cell_index, common_cell_event_t event);

  void log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name, bool warn_if_ignored = true) const;
  void log_invalid_cc(du_ue_index_t ue_idx, const char* event_name, bool warn_if_ignored = true) const;

  void handle_harq_ind(ue_cell&                               ue_cc,
                       slot_point                             uci_sl,
                       span<const mac_harq_ack_report_status> harq_bits,
                       std::optional<float>                   pucch_snr);
  void handle_csi(ue_cell& ue_cc, const csi_report_data& csi_rep);

  ue_repository&            ue_db;
  srslog::basic_logger&     logger;
  const cell_configuration& cfg;
  ue_fallback_scheduler&    fallback_sched;
  srs_scheduler&            srs_sched;
  cell_metrics_handler&     metrics;
  scheduler_event_logger&   ev_logger;

  std::unique_ptr<pdu_indication_pool> ind_pdu_pool;

  common_cell_event_queue common_events;

  slot_point last_sl_tx;

  std::atomic<bool> active = true;
};

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager final : public sched_ue_configuration_handler,
                               public scheduler_dl_buffer_state_indication_handler,
                               public scheduler_positioning_handler
{
public:
  ue_event_manager(ue_repository& ue_db);
  ~ue_event_manager() override;

  void add_cell(const cell_creation_event& cell_ev);
  void rem_cell(du_cell_index_t cell_index);
  void start_cell(du_cell_index_t cell_index);
  void stop_cell(du_cell_index_t cell_index);

  /// UE Add/Mod/Remove interface.
  void handle_ue_creation(ue_config_update_event ev) override;
  void handle_ue_reconfiguration(ue_config_update_event ev) override;
  void handle_ue_deletion(ue_config_delete_event ev) override;
  void handle_ue_config_applied(du_cell_index_t pcell_idx, du_ue_index_t ue_idx) override;

  /// Scheduler feedback handler interface.
  scheduler_feedback_handler& get_feedback_handler(du_cell_index_t cell_idx)
  {
    srsran_sanity_check(cell_exists(cell_idx), "Invalid cell index {}", fmt::underlying(cell_idx));
    return *cells[cell_idx];
  }

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

  struct common_cell_event_t {
    using callback_type = unique_function<void(), 64, true>;

    callback_type callback;
    /// UE index associated with the event. If INVALID_DU_UE_INDEX, the event is not associated with any UE.
    du_ue_index_t ue_index        = INVALID_DU_UE_INDEX;
    const char*   ev_name         = "invalid";
    bool          warn_if_ignored = true;

    common_cell_event_t() = default;
    template <typename Callable>
    common_cell_event_t(const char* ev_name_, Callable&& callable, bool warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)), ev_name(ev_name_), warn_if_ignored(warn_if_ignored_)
    {
    }
    template <typename Callable>
    common_cell_event_t(const char*   ev_name_,
                        du_ue_index_t ue_index_,
                        Callable&&    callable,
                        bool          warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)),
      ue_index(ue_index_),
      ev_name(ev_name_),
      warn_if_ignored(warn_if_ignored_)
    {
    }
  };

  using common_cell_event_queue = concurrent_queue<common_cell_event_t,
                                                   concurrent_queue_policy::lockfree_mpmc,
                                                   concurrent_queue_wait_policy::non_blocking>;

  void process_common(slot_point sl, du_cell_index_t cell_index);
  void process_cell_specific(du_cell_index_t cell_index, slot_point sl_tx);
  void clear_cell_events(du_cell_index_t cell_index);
  bool cell_exists(du_cell_index_t cell_index) const;

  void push_cell_event(du_cell_index_t cell_index, common_cell_event_t callable);

  void
  log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name = "Event", bool warn_if_ignored = true) const;

  /// List of added and configured cells.
  struct cell_context {
    const cell_configuration* cfg            = nullptr;
    cell_resource_allocator*  res_grid       = nullptr;
    cell_harq_manager*        cell_harqs     = nullptr;
    ue_fallback_scheduler*    fallback_sched = nullptr;
    uci_scheduler_impl*       uci_sched      = nullptr;
    inter_slice_scheduler*    slice_sched    = nullptr;
    srs_scheduler*            srs_sched      = nullptr;
    cell_metrics_handler*     metrics        = nullptr;
    scheduler_event_logger*   ev_logger      = nullptr;

    /// Pending cell events.
    common_cell_event_queue common_events;

    cell_context(unsigned event_qsize) : common_events(event_qsize) {}
  };

  ue_repository&        ue_db;
  srslog::basic_logger& logger;

  std::array<std::atomic<bool>, MAX_NOF_DU_CELLS>                      cell_active{true};
  std::array<std::unique_ptr<cell_context>, MAX_NOF_DU_CELLS>          du_cells;
  std::array<std::unique_ptr<ue_cell_event_manager>, MAX_NOF_DU_CELLS> cells;

  slot_point last_sl;

  std::unique_ptr<ue_dl_buffer_occupancy_manager> dl_bo_mng;
};

} // namespace srsran
