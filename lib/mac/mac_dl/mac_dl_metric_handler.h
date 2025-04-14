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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/noop_functor.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/adt/spsc_queue.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/tracing/resource_usage.h"
#include <memory>
#include <variant>

namespace srsran {

class mac_dl_metric_handler;
class mac_metrics_notifier;
class task_executor;
class timer_manager;

/// Clock time point type used for metric capture.
using metric_clock = std::chrono::high_resolution_clock;

/// Handling of MAC DL cell metrics.
class mac_dl_cell_metric_handler
{
public:
  struct slot_measurement {
    slot_measurement(mac_dl_cell_metric_handler& parent_,
                     slot_point                  sl_tx_,
                     metric_clock::time_point    slot_ind_enqueue_tp_) :
      parent(parent_.enabled() ? &parent_ : nullptr), sl_tx(sl_tx_), slot_ind_enqueue_tp(slot_ind_enqueue_tp_)
    {
      if (enabled()) {
        start_tp   = metric_clock::now();
        start_rusg = resource_usage::now();
      }
    }
    ~slot_measurement()
    {
      if (enabled()) {
        parent->handle_slot_completion(*this);
      }
    }

    bool enabled() const { return parent != nullptr; }

    auto start_time_point() const { return start_tp; }

    void on_dl_tti_req()
    {
      if (enabled()) {
        dl_tti_req_tp = metric_clock::now();
      }
    }

    void on_tx_data_req()
    {
      if (enabled()) {
        tx_data_req_tp = metric_clock::now();
      }
    }

  private:
    friend class mac_dl_cell_metric_handler;

    std::unique_ptr<mac_dl_cell_metric_handler, noop_operation> parent;
    slot_point                                                  sl_tx;
    metric_clock::time_point                                    slot_ind_enqueue_tp;
    metric_clock::time_point                                    start_tp;
    metric_clock::time_point                                    dl_tti_req_tp{};
    metric_clock::time_point                                    tx_data_req_tp{};
    expected<resource_usage::snapshot, int>                     start_rusg;
  };

  mac_dl_cell_metric_handler(mac_dl_metric_handler& parent_,
                             du_cell_index_t        cell_index,
                             pci_t                  cell_pci,
                             unsigned               period_slots_);

  /// Called when the MAC cell is activated.
  void on_cell_activation();

  /// Called when the MAC cell is deactivated.
  void on_cell_deactivation();

  /// Initiate new slot measurements.
  /// \param[in] sl_tx Tx slot.
  /// \param[in] slot_ind_trigger_tp Time point when the slot_indication was signalled by the lower layers.
  auto start_slot(slot_point sl_tx, metric_clock::time_point slot_ind_trigger_tp)
  {
    return slot_measurement{*this, sl_tx, slot_ind_trigger_tp};
  }

  bool enabled() const { return true; }

private:
  struct non_persistent_data {
    struct latency_data {
      std::chrono::nanoseconds min{std::chrono::nanoseconds::max()};
      std::chrono::nanoseconds max{0};
      std::chrono::nanoseconds sum{0};
      slot_point               max_slot;

      mac_dl_cell_metric_report::latency_report get_report(unsigned nof_slots) const;

      void save_sample(slot_point sl_tx, std::chrono::nanoseconds tdiff);
    };

    unsigned     nof_slots = 0;
    latency_data wall;
    latency_data user;
    latency_data sys;
    latency_data slot_enqueue;
    latency_data dl_tti_req;
    latency_data tx_data_req;
    unsigned     count_vol_context_switches{0};
    unsigned     count_invol_context_switches{0};
  };

  void handle_slot_completion(const slot_measurement& meas);

  const du_cell_index_t  cell_index;
  const pci_t            cell_pci;
  mac_dl_metric_handler& parent;
  const unsigned         period_slots;

  // Slot at which the next report is generated.
  slot_point               next_report_slot;
  std::chrono::nanoseconds slot_duration{0};

  // Metrics tracked
  non_persistent_data data;
};

class mac_dl_metric_handler
{
public:
  mac_dl_metric_handler(std::chrono::milliseconds period_,
                        mac_metrics_notifier&     notifier_,
                        timer_manager&            timers_,
                        task_executor&            ctrl_exec);

  /// Add new cell to be managed by the metrics handler.
  mac_dl_cell_metric_handler& add_cell(du_cell_index_t cell_index, pci_t pci, subcarrier_spacing scs);

  /// Remove cell from metric handler.
  void remove_cell(du_cell_index_t cell_index);

private:
  friend class mac_dl_cell_metric_handler;

  struct cell_start_event_type {};
  struct cell_stop_event_type {};
  using event_type = std::variant<mac_dl_cell_metric_report, cell_start_event_type, cell_stop_event_type>;
  using report_queue_type =
      concurrent_queue<event_type, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>;
  using cell_activation_bitmap_type = uint64_t;

  static_assert(sizeof(cell_activation_bitmap_type) * 8U >= MAX_NOF_DU_CELLS, "Invalid cell activation bitmap size");

  struct cell_context {
    du_cell_index_t            cell_index;
    report_queue_type          queue;
    mac_dl_cell_metric_handler handler;

    cell_context(mac_dl_metric_handler& parent, du_cell_index_t cell_index_, pci_t pci, unsigned period_slots);
  };

  void handle_cell_activation(du_cell_index_t cell_index);
  void handle_cell_deactivation(du_cell_index_t cell_index);

  void handle_cell_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& cell_report);

  void prepare_full_report();

  std::chrono::milliseconds period;
  mac_metrics_notifier&     notifier;
  timer_manager&            timers;
  task_executor&            ctrl_exec;
  srslog::basic_logger&     logger;

  slotted_array<std::unique_ptr<cell_context>, MAX_NOF_DU_CELLS> cells;
  bounded_bitset<MAX_NOF_DU_CELLS>                               active_cells_bitmap;

  // Bitmask of cells for which the report is still pending.
  std::atomic<cell_activation_bitmap_type> cell_left_bitmap{0};

  mac_metric_report next_report;
};

} // namespace srsran
