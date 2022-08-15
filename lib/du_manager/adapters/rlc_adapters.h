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

#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/rlc/rlc_entity.h"

namespace srsgnb {
namespace srs_du {

class rlc_rx_rrc_sdu_adapter : public rlc_rx_upper_layer_data_notifier
{
public:
  explicit rlc_rx_rrc_sdu_adapter(f1ap_rrc_message_transfer_procedure_handler& f1ap_rrc_) : f1ap(&f1ap_rrc_) {}

  void on_new_sdu(byte_buffer_slice pdu) override
  {
    f1_rx_pdu msg{};
    msg.pdu = std::move(pdu);
    f1ap->handle_pdu(std::move(msg));
  }

private:
  f1ap_rrc_message_transfer_procedure_handler* f1ap = nullptr;
};

class rlc_tx_data_notifier : public rlc_tx_upper_layer_data_notifier
{
public:
  void on_delivered_sdu(uint32_t pdcp_sn) override
  {
    // TODO
  }
};

class rlc_tx_control_notifier : public rlc_tx_upper_layer_control_notifier
{
public:
  void on_protocol_failure() override
  {
    // TODO
  }

  void on_max_retx() override
  {
    // TODO
  }
};

class rlc_tx_mac_buffer_state_updater : public rlc_tx_buffer_state_update_notifier
{
public:
  rlc_tx_mac_buffer_state_updater(du_ue_index_t ue_index_, lcid_t lcid_, mac_ue_control_information_handler& mac_) :
    ue_index(ue_index_), lcid(lcid_), mac(mac_)
  {
  }

  void on_buffer_state_update(unsigned bsr) override
  {
    mac_dl_bsr_indication_message msg{};
    msg.ue_index = ue_index;
    msg.lcid     = lcid;
    msg.bsr      = bsr;
    mac.handle_dl_bsr_update_required(msg);
  }

private:
  du_ue_index_t                       ue_index;
  lcid_t                              lcid;
  mac_ue_control_information_handler& mac;
};

} // namespace srs_du
} // namespace srsgnb
