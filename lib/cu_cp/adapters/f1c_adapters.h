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

#include "../../f1_interface/common/asn1_helpers.h"
#include "../ue_manager_interfaces.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1c_cu.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between F1C and CU-CP, to handle DU specific procedure outcomes (e.g. F1 Remove)
class f1c_cu_cp_adapter : public f1c_du_management_notifier
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

/// Adapter between F1C and DU processor
class f1c_du_processor_adapter : public f1c_du_processor_notifier
{
public:
  void connect_du_processor(du_processor_f1c_interface& du_processor_f1c_) { du_f1c_handler = &du_processor_f1c_; }

  du_index_t get_du_index() override { return du_f1c_handler->get_du_index(); }

  void on_f1_setup_request_received(const f1_setup_request_message& msg) override
  {
    srsgnb_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");
    du_f1c_handler->handle_f1_setup_request(msg);
  }

  ue_creation_complete_message on_create_ue(const f1ap_initial_ul_rrc_message& msg) override
  {
    srsgnb_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");

    ue_creation_message ue_creation_msg    = {};
    ue_creation_msg.c_rnti                 = to_rnti(msg.msg->c_rnti.value);
    ue_creation_msg.cgi                    = cgi_from_asn1(msg.msg->nrcgi.value);
    ue_creation_msg.du_to_cu_rrc_container = msg.msg->duto_currc_container.value;

    return du_f1c_handler->handle_ue_creation_request(ue_creation_msg);
  }

private:
  du_processor_f1c_interface* du_f1c_handler = nullptr;
};

/// Adapter between F1AP and RRC UE
class f1ap_rrc_ue_adapter : public f1c_rrc_message_notifier
{
public:
  explicit f1ap_rrc_ue_adapter(rrc_ul_ccch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer_slice pdu({rrc_container.begin(), rrc_container.end()});
    rrc_handler.handle_ul_ccch_pdu(std::move(pdu));
  }

private:
  rrc_ul_ccch_pdu_handler& rrc_handler;
};

/// Adapter between F1AP and PDCP in UL direction (Rx)
class f1ap_pdcp_adapter : public f1c_rrc_message_notifier
{
public:
  explicit f1ap_pdcp_adapter(pdcp_rx_lower_interface& pdcp_rx_) : pdcp_rx(pdcp_rx_) {}

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer pdu(rrc_container.begin(), rrc_container.end());
    pdcp_rx.handle_pdu(std::move(pdu));
  }

private:
  pdcp_rx_lower_interface& pdcp_rx;
};

} // namespace srs_cu_cp
} // namespace srsgnb
