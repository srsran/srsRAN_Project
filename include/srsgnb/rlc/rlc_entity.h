/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_RLC_ENTITY_H
#define SRSGNB_RLC_RLC_ENTITY_H

#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

/// Class used to interface with an RLC entity.
/// It will contain getters for the TX and RX entities interfaces.
class rlc_entity
{
public:
  rlc_entity()          = default;
  virtual ~rlc_entity() = default;

  virtual rlc_tx_sdu_handler*     get_tx_sdu_handler()     = 0;
  virtual rlc_tx_pdu_transmitter* get_tx_pdu_transmitter() = 0;
  virtual rlc_rx_pdu_handler*     get_rx_pdu_handler()     = 0;
};

} // namespace srsgnb
#endif
