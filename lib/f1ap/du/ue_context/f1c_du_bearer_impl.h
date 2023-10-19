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

#include "f1ap_ue_context.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1c_bearer.h"
#include "srsran/ran/rnti.h"

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
                     task_executor&             ctrl_exec_,
                     task_executor&             ue_exec_);

  /// \brief Packs and forwards the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] sdu contains the UL-CCCH message that is packed in the RRC container of the initial UL RRC message
  /// transfer message.
  void handle_sdu(byte_buffer_chain sdu) override;

  void handle_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    // TODO
  }

  void handle_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    // TODO
  }

  void handle_pdu(byte_buffer pdu) override;

private:
  f1ap_ue_context&          ue_ctxt;
  const nr_cell_global_id_t nr_cgi;
  byte_buffer               du_cu_rrc_container;
  f1ap_message_notifier&    f1ap_notifier;
  f1c_rx_sdu_notifier&      sdu_notifier;
  f1ap_event_manager&       ev_manager;
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
                          task_executor&         ctrl_exec_,
                          task_executor&         ue_exec_);

  /// \brief Packs and forwards the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] sdu The message to be encoded in the RRC container of the UL RRC message transfer message to transmit.
  void handle_sdu(byte_buffer_chain sdu) override;

  void handle_transmit_notification(uint32_t highest_pdcp_sn) override
  {
    // TODO
  }

  void handle_delivery_notification(uint32_t highest_pdcp_sn) override
  {
    // TODO
  }

  void handle_pdu(byte_buffer sdu) override;

private:
  f1ap_ue_context&       ue_ctxt;
  srb_id_t               srb_id;
  f1ap_message_notifier& f1ap_notifier;
  f1c_rx_sdu_notifier&   sdu_notifier;
  task_executor&         ctrl_exec;
  task_executor&         ue_exec;
  srslog::basic_logger&  logger;

  uint32_t get_srb_pdcp_sn(const byte_buffer& pdu);
};

} // namespace srs_du
} // namespace srsran
