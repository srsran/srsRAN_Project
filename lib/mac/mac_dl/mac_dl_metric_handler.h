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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/noop_functor.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/tracing/resource_usage.h"
#include <memory>

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

  mac_dl_cell_metric_handler(du_cell_index_t                                                        cell_index,
                             pci_t                                                                  cell_pci,
                             unsigned                                                               period_slots_,
                             std::function<void(du_cell_index_t, const mac_dl_cell_metric_report&)> on_new_cell_report);

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

  const du_cell_index_t                                                  cell_index;
  const pci_t                                                            cell_pci;
  std::function<void(du_cell_index_t, const mac_dl_cell_metric_report&)> on_new_cell_report;
  const unsigned                                                         period_slots;

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
  using report_queue_type           = concurrent_queue<mac_dl_cell_metric_report,
                                             concurrent_queue_policy::lockfree_mpmc,
                                             concurrent_queue_wait_policy::non_blocking>;
  using cell_activation_bitmap_type = unsigned;
  static_assert(sizeof(cell_activation_bitmap_type) * 8U <= MAX_NOF_DU_CELLS, "Invalid cell activation bitmap size");

  struct cell_context {
    report_queue_type          queue;
    mac_dl_cell_metric_handler handler;

    cell_context(du_cell_index_t                                                        cell_index,
                 pci_t                                                                  pci,
                 unsigned                                                               period_slots,
                 std::function<void(du_cell_index_t, const mac_dl_cell_metric_report&)> on_new_cell_report) :
      queue(2), handler(cell_index, pci, period_slots, std::move(on_new_cell_report))
    {
    }
  };

  void handle_cell_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& cell_report);

  void prepare_full_report();

  std::chrono::milliseconds period;
  mac_metrics_notifier&     notifier;
  timer_manager&            timers;
  task_executor&            ctrl_exec;

  slotted_array<std::unique_ptr<cell_context>, MAX_NOF_DU_CELLS> cells;

  std::atomic<cell_activation_bitmap_type> cell_left_bitmap{0};

  mac_metric_report next_report;
};

} // namespace srsran
