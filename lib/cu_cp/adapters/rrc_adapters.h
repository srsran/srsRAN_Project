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

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between RRC and F1AP
class f1ap_dl_message_indicator : public rrc_pdu_notifier
{
public:
  explicit f1ap_dl_message_indicator(f1ap_rrc_message_transfer_procedure_handler& f1c_handler_, ue_index_t ue_index_) :
    f1c_handler(f1c_handler_), ue_index(ue_index_)
  {
  }

  void on_new_pdu(const rrc_pdu_message msg) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = msg.srb_id;

    f1ap_msg.rrc_container.resize(msg.pdu.length());
    std::copy(msg.pdu.begin(), msg.pdu.end(), f1ap_msg.rrc_container.begin());
    f1c_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

private:
  f1ap_rrc_message_transfer_procedure_handler& f1c_handler;
  ue_index_t                                   ue_index;
};

/// Adapter between RRC and DU processor
class du_processor_rrc_ue_event_indicator : public rrc_ue_du_processor_notifier
{
public:
  void connect(du_processor_rrc_ue_interface& du_processor_rrc_ue_)
  {
    du_processor_rrc_ue_handler = &du_processor_rrc_ue_;
  }

  void on_create_srb(const srb_creation_message& msg) override { du_processor_rrc_ue_handler->create_srb(msg); }

private:
  du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
