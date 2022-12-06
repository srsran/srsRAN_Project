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

#include "srsgnb/f1u/cu_up/f1u_bearer.h"
#include "srsgnb/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsgnb/f1u/cu_up/f1u_tx_delivery_notifier.h"
#include "srsgnb/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_cu_up {

class f1u_bearer_impl final : public f1u_bearer, public f1u_rx_pdu_handler, public f1u_tx_sdu_handler
{
public:
  f1u_bearer_impl(drb_id_t                  drb_id_,
                  f1u_tx_pdu_notifier&      tx_pdu_notifier_,
                  f1u_tx_delivery_notifier& tx_delivery_notifier_,
                  f1u_rx_sdu_notifier&      rx_sdu_notifier_) :
    drb_id(drb_id_),
    tx_pdu_notifier(tx_pdu_notifier_),
    tx_delivery_notifier(tx_delivery_notifier_),
    rx_sdu_notifier(rx_sdu_notifier_)
  {
  }

  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() override { return *this; }
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() override { return *this; }

  void handle_pdu(byte_buffer pdu) override;
  void handle_sdu(byte_buffer sdu, uint32_t count) override;
  void discard_sdu(uint32_t count) override;

private:
  drb_id_t                  drb_id;
  f1u_tx_pdu_notifier&      tx_pdu_notifier;
  f1u_tx_delivery_notifier& tx_delivery_notifier;
  f1u_rx_sdu_notifier&      rx_sdu_notifier;
};

} // namespace srs_cu_up
} // namespace srsgnb
