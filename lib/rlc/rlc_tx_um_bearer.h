/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_TX_UM_BEARER_H
#define SRSGNB_RLC_TX_UM_BEARER_H

#include "srsgnb/rlc/rlc_common_bearer.h"

namespace srsgnb {

class rlc_tx_um_bearer : public rlc_tx_common_bearer
{
public:
  rlc_tx_um_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    rlc_tx_common_bearer(du_index, lcid, upper_cn)
  {}

  /*
   * Interfaces for higher layers
   */
  void handle_sdu(byte_buffer sdu) override
  {
    // TODO
  }

  /*
   * Interfaces for lower layers
   */
  bool pull_pdu(byte_buffer& pdu, uint32_t nof_bytes) override
  {
    // TODO
    return true;
  }

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
