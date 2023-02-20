/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../f1ap/common/asn1_helpers.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between F1AP and CU-CP, to handle DU specific procedure outcomes (e.g. F1 Remove)
class f1ap_cu_cp_adapter : public f1ap_du_management_notifier
{
public:
  void connect_cu_cp(cu_cp_du_handler& cu_cp_mng_) { du_handler = &cu_cp_mng_; }

  void on_du_remove_request_received(const du_index_t du_index) override
  {
    srsgnb_assert(du_handler != nullptr, "DU handler must not be nullptr");
    du_handler->handle_du_remove_request(du_index);
  }

private:
  cu_cp_du_handler* du_handler = nullptr;
};

/// Adapter between F1AP and DU processor
class f1ap_du_processor_adapter : public f1ap_du_processor_notifier
{
public:
  void connect_du_processor(du_processor_f1ap_interface& du_processor_f1ap_) { du_f1ap_handler = &du_processor_f1ap_; }

  du_index_t get_du_index() override { return du_f1ap_handler->get_du_index(); }

  void on_f1_setup_request_received(const f1_setup_request_message& msg) override
  {
    srsgnb_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    du_f1ap_handler->handle_f1_setup_request(msg);
  }

  ue_creation_complete_message on_create_ue(const f1ap_initial_ul_rrc_message& msg) override
  {
    srsgnb_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");

    ue_creation_message ue_creation_msg    = {};
    ue_creation_msg.c_rnti                 = to_rnti(msg.msg->c_rnti.value);
    ue_creation_msg.cgi                    = cgi_from_asn1(msg.msg->nr_cgi.value);
    ue_creation_msg.du_to_cu_rrc_container = msg.msg->du_to_cu_rrc_container.value;

    return du_f1ap_handler->handle_ue_creation_request(ue_creation_msg);
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
    pdcp_rx.handle_pdu(byte_buffer_slice_chain{std::move(pdu)});
  }

private:
  pdcp_rx_lower_interface& pdcp_rx;
};

} // namespace srs_cu_cp
} // namespace srsran
