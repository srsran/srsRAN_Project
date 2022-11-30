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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/ran/bcd_helpers.h"
#include "srsgnb/rrc/rrc_ue.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between NGC and AMF
class ngc_amf_adapter : public ngc_message_notifier
{
public:
  void on_new_message(const ngc_message& msg) override
  {
    // TODO: Add network socket transmit messages to AMF
  }
};

// Adapter between NGC and RRC UE
class ngc_rrc_ue_adapter : public ngc_rrc_ue_pdu_notifier, public ngc_rrc_ue_control_notifier
{
public:
  void connect_rrc_ue(rrc_ue_dl_nas_message_handler*  rrc_ue_msg_handler_,
                      rrc_ue_control_message_handler* rrc_ue_ctrl_handler_)
  {
    rrc_ue_msg_handler  = rrc_ue_msg_handler_;
    rrc_ue_ctrl_handler = rrc_ue_ctrl_handler_;
  }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    srsgnb_assert(rrc_ue_msg_handler != nullptr, "rrc_ue_msg_handler must not be nullptr");

    dl_nas_transport_message dl_nas_msg = {};
    dl_nas_msg.nas_pdu                  = std::move(nas_pdu);

    rrc_ue_msg_handler->handle_dl_nas_transport_message(dl_nas_msg);
  }

  void on_initial_context_setup_request_received(const ngap_initial_context_setup_request_message& msg) override
  {
    srsgnb_assert(rrc_ue_ctrl_handler != nullptr, "rrc_ue_ctrl_handler must not be nullptr");

    rrc_reconfiguration_request_message rrc_reconf_req_msg = {};
    rrc_reconf_req_msg.cu_cp_ue_id                         = msg.cu_cp_ue_id;

    rrc_reconf_req_msg.plmn = plmn_bcd_to_string(msg.request->guami->plmn_id.to_number());

    rrc_ue_ctrl_handler->handle_rrc_reconfiguration_request(rrc_reconf_req_msg);
  }

private:
  rrc_ue_dl_nas_message_handler*  rrc_ue_msg_handler  = nullptr;
  rrc_ue_control_message_handler* rrc_ue_ctrl_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
