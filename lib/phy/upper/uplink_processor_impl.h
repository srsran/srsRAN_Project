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

#include "rx_payload_buffer_pool.h"
#include "uplink_pdu_slot_repository_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/uplink_slot_processor.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include <utility>

namespace srsran {
namespace detail {
/// \brief Adapts the PUSCH processor result notifier to the upper PHY receive results notifier.
///
/// It collects the Channel State Information (CSI), control and data decoding information from a PUSCH processor object
/// and notifies them through the upper PHY notification interface.
///
/// \remark The order of the calls matters. Method on_csi() must be called before on_uci() or on_sch(). Otherwise, an
/// assertion is triggered.
class pusch_processor_result_notifier_adaptor : private pusch_processor_result_notifier
{
public:
  /// Callback function type.
  using callback_func = unique_function<void(), default_unique_task_buffer_size, true>;

  /// \brief Creates a PUSCH processor result notifier adaptor.
  /// \param[in] notifier_   Upper physical layer result notifier.
  /// \param[in] rnti_       User RNTI.
  /// \param[in] slot_       Current slot.
  /// \param[in] harq_id_    User HARQ process identifier.
  /// \param[in] payload_    View to the data payload.
  /// \param[in] callback_   Callback function when the PUSCH transmission is completed.
  pusch_processor_result_notifier& configure(upper_phy_rx_results_notifier& notifier_,
                                             rnti_t                         rnti_,
                                             slot_point                     slot_,
                                             harq_id_t                      harq_id_,
                                             span<const uint8_t>            payload_,
                                             callback_func                  callback_)
  {
    notifier = &notifier_;
    rnti     = rnti_;
    slot     = slot_;
    harq_id  = harq_id_;
    payload  = payload_;
    callback = std::move(callback_);
    return *this;
  }

private:
  // See interface for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(notifier != nullptr, "Invalid notifier.");

    ul_pusch_results_control result;
    result.rnti = rnti;
    result.slot = slot;
    result.csi  = uci.csi;

    if (!uci.harq_ack.payload.empty()) {
      result.harq_ack.emplace(uci.harq_ack);
    }

    if (!uci.csi_part1.payload.empty()) {
      result.csi1.emplace(uci.csi_part1);
    }

    if (!uci.csi_part2.payload.empty()) {
      result.csi2.emplace(uci.csi_part2);
    }

    notifier->on_new_pusch_results_control(result);

    l1_tracer << instant_trace_event("on_uci", instant_trace_event::cpu_scope::thread);
  }

  // See interface for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    // Gets the notifier and invalidates. It makes sure the notifier is no longer available after the exchange.
    upper_phy_rx_results_notifier* notifier_ = nullptr;
    std::exchange(notifier_, notifier);
    srsran_assert(notifier_ != nullptr, "Invalid notifier.");

    // Notify the completion of PUSCH data.
    ul_pusch_results_data result;
    result.rnti           = rnti;
    result.slot           = slot;
    result.csi            = sch.csi;
    result.harq_id        = harq_id;
    result.decoder_result = sch.data;
    result.payload        = (sch.data.tb_crc_ok) ? payload : span<const uint8_t>();
    notifier_->on_new_pusch_results_data(result);

    l1_tracer << instant_trace_event("on_sch", instant_trace_event::cpu_scope::thread);

    if (!callback.is_empty()) {
      callback();
    }
  }

  upper_phy_rx_results_notifier* notifier = nullptr;
  rnti_t                         rnti     = rnti_t::INVALID_RNTI;
  slot_point                     slot     = slot_point();
  harq_id_t                      harq_id  = harq_id_t::INVALID_HARQ_ID;
  span<const uint8_t>            payload;
  callback_func                  callback;
};

} // namespace detail

class task_executor;

/// \brief Uplink processor implementation.
///
/// In this implementation, every task is processed by its channel processor. When a task finishes, the results are
/// notified by the upper_phy_rx_results_notifier interface.
class uplink_processor_impl : public uplink_processor, private uplink_slot_processor
{
public:
  /// Wraps a task executor that forbids memory allocation by design.
  class uplink_task_executor
  {
  public:
    using task_type = unique_function<void(), default_unique_task_buffer_size, true>;

    uplink_task_executor(task_executor& executor_) : executor(executor_) {}

    uplink_task_executor(const uplink_task_executor& other) : executor(other.executor) {}

    uplink_task_executor(uplink_task_executor&& other) : executor(other.executor) {}

    bool execute(task_type&& task) { return executor.execute(std::move(task)); }

  private:
    task_executor& executor;
  };

  /// Collects task executors.
  struct task_executor_collection {
    /// Executor for the PUCCH tasks generated within this uplink processor.
    uplink_task_executor pucch_executor;
    /// Executor for the PUSCH tasks generated within this uplink processor.
    uplink_task_executor pusch_executor;
    /// Executor for the SRS tasks generated within this uplink processor.
    uplink_task_executor srs_executor;
    /// Executor for the PRACH tasks generated within this uplink processor.
    uplink_task_executor prach_executor;
  };

  uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                        std::unique_ptr<pusch_processor> pusch_proc_,
                        std::unique_ptr<pucch_processor> pucch_proc_,
                        std::unique_ptr<srs_estimator>   srs_,
                        task_executor_collection&        task_executors_,
                        rx_buffer_pool&                  rm_buffer_pool_,
                        upper_phy_rx_results_notifier&   notifier_,
                        unsigned                         max_nof_prb,
                        unsigned                         max_nof_layers);

  // See uplink_processor interface for documentation.
  unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) override;

  // See uplink_processor interface for documentation.
  uplink_slot_processor& get_slot_processor() override;

private:
  // See uplink_slot_processor interface for documentation.
  void handle_rx_symbol(const shared_resource_grid& grid, unsigned end_symbol_index) override;

  // See uplink_slot_processor interface for documentation.
  void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) override;

  // See uplink_slot_processor interface for documentation.
  void discard_slot() override;

  /// Helper method for processing PUSCH.
  void process_pusch(const shared_resource_grid& grid, const uplink_pdu_slot_repository::pusch_pdu& pdu);

  /// Helper method for processing PUCCH.
  void process_pucch(const shared_resource_grid& grid, const uplink_pdu_slot_repository::pucch_pdu& pdu);

  /// Helper method for processing SRS.
  void process_srs(const shared_resource_grid& grid, const uplink_pdu_slot_repository::srs_pdu& pdu);

  /// Helper method for notifying a discarded PUSCH reception.
  void notify_discard_pusch(const uplink_pdu_slot_repository::pusch_pdu& pdu);

  /// Helper method for notifying a discarded PUCCH reception.
  void notify_discard_pucch(const uplink_pdu_slot_repository::pucch_pdu& pdu);

  /// PDU repository.
  uplink_pdu_slot_repository_impl pdu_repository;
  /// Pool of PUSCH processor adaptors.
  std::array<detail::pusch_processor_result_notifier_adaptor, MAX_PUSCH_PDUS_PER_SLOT> pusch_adaptors;
  /// Counter of used PUSCH adaptors. It shall not exceed \c MAX_PUSCH_PDUS_PER_SLOT.
  std::atomic<unsigned> count_pusch_adaptors;
  /// PRACH detector.
  std::unique_ptr<prach_detector> prach;
  /// PUSCH processor.
  std::unique_ptr<pusch_processor> pusch_proc;
  /// PUCCH processor.
  std::unique_ptr<pucch_processor> pucch_proc;
  /// SRS channel estimator.
  std::unique_ptr<srs_estimator> srs;
  /// Task executors.
  task_executor_collection task_executors;
  /// Receive buffer pool.
  rx_buffer_pool& rm_buffer_pool;
  /// Pool of containers for the payload.
  rx_payload_buffer_pool rx_payload_pool;
  /// General PHY logger, used to notify pool failures.
  srslog::basic_logger& logger;
  /// Slot context.
  slot_point current_slot;
  /// Upper physical layer receive results notifier reference.
  upper_phy_rx_results_notifier& notifier;
};
} // namespace srsran
