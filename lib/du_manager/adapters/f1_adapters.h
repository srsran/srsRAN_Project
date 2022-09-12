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

#include "srsgnb/f1_interface/du/f1_bearer.h"
#include "srsgnb/rlc/rlc_tx.h"

namespace srsgnb {
namespace srs_du {

class f1_tx_rlc_pdu_adapter final : public f1_tx_pdu_notifier
{
public:
  void connect(rlc_tx_upper_layer_data_interface& rlc_tx_) { rlc_tx = &rlc_tx_; }

  void on_tx_pdu(f1_tx_pdu pdu) override
  {
    srsgnb_assert(rlc_tx != nullptr, "MAC Tx PDU notifier is disconnected");
    rlc_tx->handle_sdu(rlc_sdu{pdu.pdcp_count, std::move(pdu.pdu)});
  }

private:
  rlc_tx_upper_layer_data_interface* rlc_tx = nullptr;
};

} // namespace srs_du
} // namespace srsgnb