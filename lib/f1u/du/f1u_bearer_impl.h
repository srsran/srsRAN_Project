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

#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_du {

class f1u_bearer_impl final : public f1u_bearer,
                              public f1u_tx_sdu_handler,
                              public f1u_tx_delivery_handler,
                              public f1u_rx_pdu_handler
{
public:
  f1u_bearer_impl(drb_id_t drb_id_, f1u_rx_sdu_notifier& rx_sdu_notifier_);

  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }

  void handle_sdu(byte_buffer_slice_chain sdu) override;
  void handle_delivered_sdu(uint32_t count) override;
  void handle_pdu(nru_dl_message msg) override;

private:
  drb_id_t             drb_id;
  f1u_rx_sdu_notifier& rx_sdu_notifier;
};

} // namespace srs_du
} // namespace srsgnb
