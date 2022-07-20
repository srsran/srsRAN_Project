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

/// Adapter between DU processor and RRC
class rrc_ul_ccch_message_indicator : public du_processor_rrc_message_notifier
{
public:
  explicit rrc_ul_ccch_message_indicator(rrc_ul_ccch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer_slice pdu({rrc_container.begin(), rrc_container.end()});
    rrc_handler.handle_ul_ccch_pdu(std::move(pdu));
  }

private:
  rrc_ul_ccch_pdu_handler& rrc_handler;
};

/// Adapter between DU processor and F1AP
class f1ap_du_processor_event_indicator : public du_processor_f1c_message_notifier
{
public:
  void connect(f1ap_rrc_message_transfer_procedure_handler& f1c_handler_) { f1c_handler = &f1c_handler_; }

  void on_ue_creation(unsigned int tmp_ue_id, ue_index_t ue_index) override
  {
    f1c_handler->add_ue_index_to_context(int_to_f1ap_ue_id(tmp_ue_id), ue_index);
  }

private:
  f1ap_rrc_message_transfer_procedure_handler* f1c_handler;
};

} // namespace srs_cu_cp
} // namespace srsgnb
