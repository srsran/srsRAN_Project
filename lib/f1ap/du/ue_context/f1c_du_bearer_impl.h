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

#include "f1ap_ue_context.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1c_bearer.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/async/protocol_transaction_manager.h"
#include "srsran/support/memory_pool/unsync_fixed_size_memory_block_pool.h"
#include <deque>

namespace srsran {
namespace srs_du {

class f1ap_event_manager;

class f1c_srb0_du_bearer final : public f1c_bearer
{
public:
  f1c_srb0_du_bearer(f1ap_ue_context&           ue_ctxt_,
                     const nr_cell_global_id_t& pcell_cgi,
                     const byte_buffer&         du_cu_rrc_container_,
                     f1ap_message_notifier&     f1ap_notifier_,
                     f1c_rx_sdu_notifier&       f1c_rx_sdu_notifier_,
                     f1ap_event_manager&        ev_manager_,
                     f1ap_du_configurator&      du_configurator_,
                     task_executor&             ctrl_exec_,
                     task_executor&             ue_exec_);

  /// \brief Packs and forwards the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] sdu contains the UL-CCCH message that is packed in the RRC container of the initial UL RRC message
  /// transfer message.
  void handle_sdu(byte_buffer_chain sdu) override;

  void handle_transmit_notification(uint32_t highest_pdcp_sn) override;
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override;

  void             handle_pdu(byte_buffer pdu, bool rrc_delivery_status_request = false) override;
  async_task<bool> handle_pdu_and_await_delivery(byte_buffer               sdu,
                                                 bool                      report_rrc_delivery_status,
                                                 std::chrono::milliseconds time_to_wait) override;
  async_task<bool> handle_pdu_and_await_transmission(byte_buffer               pdu,
                                                     bool                      report_rrc_delivery_status,
                                                     std::chrono::milliseconds time_to_wait) override;

private:
  f1ap_ue_context&          ue_ctxt;
  const nr_cell_global_id_t nr_cgi;
  byte_buffer               du_cu_rrc_container;
  f1ap_message_notifier&    f1ap_notifier;
  f1c_rx_sdu_notifier&      sdu_notifier;
  f1ap_event_manager&       ev_manager;
  f1ap_du_configurator&     du_configurator;
  task_executor&            ctrl_exec;
  task_executor&            ue_exec;
  srslog::basic_logger&     logger;
};

class f1c_other_srb_du_bearer final : public f1c_bearer
{
public:
  f1c_other_srb_du_bearer(f1ap_ue_context&       ue_ctxt_,
                          srb_id_t               srb_id_,
                          f1ap_message_notifier& f1ap_notifier_,
                          f1c_rx_sdu_notifier&   f1c_sdu_notifier_,
                          f1ap_du_configurator&  du_configurator_,
                          task_executor&         ctrl_exec_,
                          task_executor&         ue_exec_,
                          timer_manager&         timers_);

  /// \brief Packs and forwards the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] sdu The message to be encoded in the RRC container of the UL RRC message transfer message to transmit.
  void handle_sdu(byte_buffer_chain sdu) override;

  void handle_transmit_notification(uint32_t highest_pdcp_sn) override;
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override;

  void             handle_pdu(byte_buffer sdu, bool rrc_delivery_status_request = false) override;
  async_task<bool> handle_pdu_and_await_delivery(byte_buffer               pdu,
                                                 bool                      report_rrc_delivery_status,
                                                 std::chrono::milliseconds time_to_wait) override;
  async_task<bool> handle_pdu_and_await_transmission(byte_buffer               pdu,
                                                     bool                      report_rrc_delivery_status,
                                                     std::chrono::milliseconds time_to_wait) override;

private:
  using event_source_type   = protocol_transaction_event_source<bool>;
  using event_observer_type = protocol_transaction_outcome_observer<bool>;
  using event_result_type   = std::variant<bool, no_fail_response_path, protocol_transaction_failure>;

  struct event_context {
    uint32_t            pdcp_sn;
    event_source_type   source;
    event_observer_type observer;

    event_context(uint32_t pdcp_sn_, timer_factory timers, std::chrono::milliseconds time_to_wait) :
      pdcp_sn(pdcp_sn_), source(timers)
    {
      observer.subscribe_to(source, time_to_wait);
    }
  };
  using event_ptr = unsync_fixed_size_object_pool<event_context>::ptr;

  async_task<bool> handle_pdu_and_await(byte_buffer               pdu,
                                        bool                      tx_or_delivery,
                                        bool                      report_rrc_delivery_status,
                                        std::chrono::milliseconds time_to_wait);
  event_observer_type&
       wait_for_notification(uint32_t pdcp_sn, bool tx_or_delivery, std::chrono::milliseconds time_to_wait);
  void handle_notification(uint32_t highest_pdcp_sn, bool tx_or_delivery);

  /// Handle RRC Delivery Report as per TS 38.473, Section 8.4.4.
  void handle_rrc_delivery_report(uint32_t trigger_pdcp_sn, uint32_t highest_in_order_pdcp_sn);

  f1ap_ue_context&       ue_ctxt;
  srb_id_t               srb_id;
  f1ap_message_notifier& f1ap_notifier;
  f1c_rx_sdu_notifier&   sdu_notifier;
  f1ap_du_configurator&  du_configurator;
  task_executor&         ctrl_exec;
  task_executor&         ue_exec;
  timer_manager&         timers;
  srslog::basic_logger&  logger;
  event_observer_type    always_set_event;

  std::optional<uint32_t> last_pdcp_sn_transmitted;
  std::optional<uint32_t> last_pdcp_sn_delivered;

  // List of pending RRC delivery status reports for this SRB.
  std::deque<uint32_t> pending_rrc_delivery_status_reports;

  // Pool of events for PDU transmission and delivery. Each entry is represented by the PDCP SN (negative if
  // the pool element is negative) and the event flag to wait for.
  unsync_fixed_size_object_pool<event_context> event_pool;
  std::vector<event_ptr>                       pending_deliveries;
  std::vector<event_ptr>                       pending_transmissions;
};

} // namespace srs_du
} // namespace srsran
