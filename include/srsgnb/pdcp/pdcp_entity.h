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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/pdcp/pdcp.h"

namespace srsgnb {

/// Interface for the PDCP bearer.
/// Provides getters and setters for the RX and TX parts of the PDCP entity.
class pdcp_entity
{
public:
  virtual ~pdcp_entity() = default;

  virtual pdcp_tx_sdu_handler*              get_tx_sdu_handler()              = 0;
  virtual pdcp_tx_lower_layer_data_handler* get_tx_lower_layer_data_handler() = 0;
  virtual pdcp_rx_pdu_handler*              get_rx_pdu_handler()              = 0;
};

} // namespace srsgnb
