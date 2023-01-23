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

#include "f1u_bearer_logger.h"
#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_du {

class f1u_bearer_impl final : public f1u_bearer,
                              public f1u_tx_sdu_handler,
                              public f1u_tx_delivery_handler,
                              public f1u_rx_pdu_handler
{
public:
  f1u_bearer_impl(uint32_t             ue_index,
                  drb_id_t             drb_id_,
                  f1u_rx_sdu_notifier& rx_sdu_notifier_,
                  f1u_tx_pdu_notifier& tx_pdu_notifier_);

  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }

  void handle_sdu(byte_buffer_slice_chain sdu) override;
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override;
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override;
  void handle_pdu(nru_dl_message msg) override;

private:
  f1u_bearer_logger    logger;
  f1u_rx_sdu_notifier& rx_sdu_notifier;
  f1u_tx_pdu_notifier& tx_pdu_notifier;
};

} // namespace srs_du
} // namespace srsgnb
