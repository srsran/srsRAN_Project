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

#include "srsgnb/f1c/du/f1c_bearer.h"
#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/mac/mac_ue_control_information_handler.h"
#include "srsgnb/rlc/rlc_entity.h"

namespace srsgnb {
namespace srs_du {

class rlc_rx_rrc_sdu_adapter : public rlc_rx_upper_layer_data_notifier
{
public:
  void connect(f1c_bearer& bearer_) { f1bearer = &bearer_; }

  void on_new_sdu(byte_buffer_slice_chain pdu) override
  {
    srsgnb_assert(f1bearer != nullptr, "rlc rx bearer notifier is disconnected");
    f1bearer->handle_pdu(std::move(pdu));
  }

private:
  f1c_bearer* f1bearer = nullptr;
};

class rlc_f1u_rx_sdu_adapter : public rlc_rx_upper_layer_data_notifier
{
public:
  void connect(f1u_rx_pdu_handler& bearer_) { f1bearer = &bearer_; }

  void on_new_sdu(byte_buffer_slice_chain pdu) override
  {
    srsgnb_assert(f1bearer != nullptr, "rlc rx bearer notifier is disconnected");
    f1bearer->handle_pdu(std::move(pdu));
  }

private:
  f1u_rx_pdu_handler* f1bearer = nullptr;
};

class rlc_tx_data_notifier : public rlc_tx_upper_layer_data_notifier
{
public:
  void on_delivered_sdu(uint32_t pdcp_count) override
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

class rlc_tx_mac_buffer_state_updater : public rlc_tx_lower_layer_notifier
{
public:
  void connect(du_ue_index_t ue_index_, lcid_t lcid_, mac_ue_control_information_handler& mac_)
  {
    ue_index = ue_index_;
    lcid     = lcid_;
    mac      = &mac_;
  }

  void on_buffer_state_update(unsigned bsr) override
  {
    srsgnb_assert(mac != nullptr, "RLC Tx Buffer State notifier is disconnected");
    mac_dl_buffer_state_indication_message bs{};
    bs.ue_index = ue_index;
    bs.lcid     = lcid;
    bs.bs       = bsr;
    mac->handle_dl_buffer_state_update_required(bs);
  }

private:
  du_ue_index_t                       ue_index = INVALID_DU_UE_INDEX;
  lcid_t                              lcid     = INVALID_LCID;
  mac_ue_control_information_handler* mac      = nullptr;
};

} // namespace srs_du
} // namespace srsgnb
