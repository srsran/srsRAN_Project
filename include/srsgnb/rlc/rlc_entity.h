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

#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"

namespace srsgnb {

/// Class used to interface with an RLC entity.
/// It will contain getters for the TX and RX entities interfaces.
class rlc_entity
{
public:
  rlc_entity()          = default;
  virtual ~rlc_entity() = default;

  virtual rlc_tx_upper_data_interface* get_tx_upper_data_interface() = 0;
  virtual rlc_tx_lower_interface*      get_tx_lower_interface()      = 0;
  virtual rlc_rx_lower_interface*      get_rx_lower_interface()      = 0;
};

} // namespace srsgnb
