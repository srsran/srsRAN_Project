/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_TX_UM_ENTITY_H
#define SRSGNB_RLC_TX_UM_ENTITY_H

#include "rlc_tx_entity.h"

namespace srsgnb {

class rlc_tx_um_entity : public rlc_tx_entity
{
public:
  rlc_tx_um_entity(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_notifier& upper_cn) :
    rlc_tx_entity(du_index, lcid, upper_cn)
  {
  }

  /*
   * Interfaces for higher layers
   */
  void handle_sdu(rlc_sdu sdu) override
  {
    // TODO
  }

  /*
   * Interfaces for lower layers
   */
  rlc_byte_buffer pull_pdu(uint32_t nof_bytes) override
  {
    // TODO
    return rlc_byte_buffer{};
  }

  void get_buffer_state(uint32_t& bytes) override
  {
    // TODO
    bytes = 0;
  }
};

} // namespace srsgnb

#endif
