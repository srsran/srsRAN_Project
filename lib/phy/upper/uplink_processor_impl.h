/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

namespace srsran {

namespace detail {

/// Concurrent queue type used for storing the PUSCH processor result notifier adaptors.
using free_adaptor_queue =
    concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>;

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
  /// \brief Constructs an adaptor.
  ///
  /// The PUSCH adaptor queue identifier is set to the current size of the queue.
  ///
  /// \param[in] queue_ Free adaptor queue instance.
  pusch_processor_result_notifier_adaptor(free_adaptor_queue& queue_) : queue(queue_), queue_identifier(queue.size())
  {
    queue.push_blocking(queue_identifier);
  }

  /// \brief Creates a PUSCH processor result notifier adaptor.
  /// \param[in] notifier_   Upper physical layer result notifier.
  /// \param[in] rnti_       User RNTI.
  /// \param[in] slot_       Current slot.
  /// \param[in] harq_id_    User HARQ process identifier.
  /// \param[in] payload_    View to the data payload.
  pusch_processor_result_notifier& configure(upper_phy_rx_results_notifier& notifier_,
                                             rnti_t                         rnti_,
                                             slot_point                     slot_,
                                             harq_id_t                      harq_id_,
                                             span<const uint8_t>            payload_)
  {
    notifier = &notifier_;
    rnti     = rnti_;
    slot     = slot_;
    harq_id  = harq_id_;
    payload  = payload_;
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

    // Return the adaptor identifier to the queue.
    queue.push_blocking(queue_identifier);

    l1_tracer << instant_trace_event("on_sch", instant_trace_event::cpu_scope::thread);
  }

  free_adaptor_queue&            queue;
  unsigned                       queue_identifier;
  upper_phy_rx_results_notifier* notifier = nullptr;
  rnti_t                         rnti     = rnti_t::INVALID_RNTI;
  slot_point                     slot     = slot_point();
  harq_id_t                      harq_id  = harq_id_t::INVALID_HARQ_ID;
  span<const uint8_t>            payload;
};

} // namespace detail

class task_executor;

/// \brief Uplink processor implementation.
///
/// In this implementation, every task is processed by its channel processor. When a task finishes, the results are
/// notified by the upper_phy_rx_results_notifier interface.
class uplink_processor_impl : public uplink_processor
{
public:
  uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                        std::unique_ptr<pusch_processor> pusch_proc_,
                        std::unique_ptr<pucch_processor> pucch_proc_);

  // See interface for documentation.
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override;

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_buffer                   rm_buffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override;

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override;

private:
  /// Maximum number of PUSCH notifier adaptors.
  static constexpr unsigned max_nof_pusch_notifier_adaptors = 1024;
  /// Pool of adaptors.
  std::vector<detail::pusch_processor_result_notifier_adaptor> pusch_adaptors;
  /// Queue of free adaptors.
  detail::free_adaptor_queue free_pusch_adaptors;
  /// PRACH detector.
  std::unique_ptr<prach_detector> prach;
  /// PUSCH processor.
  std::unique_ptr<pusch_processor> pusch_proc;
  /// PUCCH processor.
  std::unique_ptr<pucch_processor> pucch_proc;
  /// General PHY logger, used to notify pool failures.
  srslog::basic_logger& logger;
};

} // namespace srsran
