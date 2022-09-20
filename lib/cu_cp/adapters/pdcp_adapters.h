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

#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between PDCP and RRC in UL direction (Rx)
class pdcp_rrc_ue_adapter : public pdcp_rx_upper_data_notifier
{
public:
  explicit pdcp_rrc_ue_adapter(rrc_ul_dcch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_sdu(byte_buffer sdu) override { rrc_handler.handle_ul_dcch_pdu(std::move(sdu)); }

private:
  rrc_ul_dcch_pdu_handler& rrc_handler;
};

/// Adapter between PDCP and DU processor for DL PDUs
class pdcp_du_processor_adapter : public pdcp_tx_lower_notifier
{
public:
  explicit pdcp_du_processor_adapter(f1c_rrc_message_handler& f1c_handler_,
                                     const ue_index_t&        ue_index_,
                                     const srb_id_t&          srb_id_) :
    f1c_handler(f1c_handler_), ue_index(ue_index_), srb_id(srb_id_)
  {
  }

  void on_new_pdu(byte_buffer pdu) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = srb_id;

    f1ap_msg.rrc_container.resize(pdu.length());
    std::copy(pdu.begin(), pdu.end(), f1ap_msg.rrc_container.begin());
    f1c_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

  void on_discard_pdu(uint32_t count) override
  {
    // not implemented
  }

private:
  f1c_rrc_message_handler& f1c_handler;
  const ue_index_t         ue_index;
  const srb_id_t           srb_id;
};

} // namespace srs_cu_cp
} // namespace srsgnb
