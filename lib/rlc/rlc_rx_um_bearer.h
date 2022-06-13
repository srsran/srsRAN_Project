/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_RX_UM_BEARER_H
#define SRSGNB_RLC_RX_UM_BEARER_H

#include "srsgnb/rlc/rlc_common_bearer.h"

namespace srsgnb {

class rlc_rx_um_bearer : public rlc_rx_common_bearer
{
public:
  rlc_rx_um_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_rx_upper_layer_data_notifier& upper_dn) :
    rlc_rx_common_bearer(du_index, lcid, upper_dn)
  {
  }

  void handle_pdu(byte_buffer buf) override { upper_dn.on_new_sdu(std::move(buf)); }
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
