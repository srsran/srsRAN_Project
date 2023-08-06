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

#include "../../f1ap/common/asn1_helpers.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/pdcp/pdcp_rx.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between F1AP and CU-CP, to handle DU specific procedure outcomes (e.g. F1 Remove)
class f1ap_cu_cp_adapter : public f1ap_du_management_notifier
{
public:
  void connect_cu_cp(du_repository& cu_cp_mng_) { du_handler = &cu_cp_mng_; }

  void on_du_remove_request_received(const du_index_t du_index) override
  {
    srsran_assert(du_handler != nullptr, "DU handler must not be nullptr");
    du_handler->handle_du_remove_request(du_index);
  }

private:
  du_repository* du_handler = nullptr;
};

/// Adapter between F1AP and DU processor
class f1ap_du_processor_adapter : public f1ap_du_processor_notifier
{
public:
  void connect_du_processor(du_processor_f1ap_interface& du_processor_f1ap_) { du_f1ap_handler = &du_processor_f1ap_; }

  du_index_t get_du_index() override { return du_f1ap_handler->get_du_index(); }

  void on_f1_setup_request_received(const f1ap_f1_setup_request& msg) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    du_f1ap_handler->handle_f1_setup_request(msg);
  }

  ue_index_t on_new_ue_index_required() override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    return du_f1ap_handler->get_new_ue_index();
  }

  ue_creation_complete_message on_create_ue(const cu_cp_ue_creation_message& msg) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    return du_f1ap_handler->handle_ue_creation_request(msg);
  }

  void on_delete_ue(ue_index_t ue_index) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    du_f1ap_handler->remove_ue(ue_index);
  }

  void on_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& req) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    du_f1ap_handler->handle_du_initiated_ue_context_release_request(req);
  }

private:
  du_processor_f1ap_interface* du_f1ap_handler = nullptr;
};

/// Adapter between F1AP and RRC UE
class f1ap_rrc_ue_adapter : public f1ap_rrc_message_notifier
{
public:
  explicit f1ap_rrc_ue_adapter(rrc_ul_ccch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer_slice pdu(byte_buffer{rrc_container.begin(), rrc_container.end()});
    rrc_handler.handle_ul_ccch_pdu(std::move(pdu));
  }

private:
  rrc_ul_ccch_pdu_handler& rrc_handler;
};

/// Adapter between F1AP and PDCP in UL direction (Rx)
class f1ap_pdcp_adapter : public f1ap_rrc_message_notifier
{
public:
  explicit f1ap_pdcp_adapter(pdcp_rx_lower_interface& pdcp_rx_) : pdcp_rx(pdcp_rx_) {}

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer pdu(rrc_container.begin(), rrc_container.end());
    pdcp_rx.handle_pdu(byte_buffer_chain{std::move(pdu)});
  }

private:
  pdcp_rx_lower_interface& pdcp_rx;
};

} // namespace srs_cu_cp
} // namespace srsran
