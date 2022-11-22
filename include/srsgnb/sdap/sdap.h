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

namespace srsgnb {

namespace srs_cu_up {

// :TODO: These are the main input/output interfaces to pass data traffic between layers. As the implementation
// progresses on we'll add additional files that include new interfaces for configuration and other responsibilities.
// Please note these interfaces represent a single bearer or SDAP entity.

/// This interface represents the entry point of the receiving side of a SDAP entity.
class sdap_pdu_handler
{
public:
  virtual ~sdap_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_pdu(byte_buffer pdu) = 0;
};

/// This interface notifies to upper layers the reception of new SDUs in the receiving side of a SDAP entity.
class sdap_sdu_rx_notifier
{
public:
  virtual ~sdap_sdu_rx_notifier() = default;

  /// This callback is invoked on each generated SDU.
  virtual void on_new_sdu(byte_buffer sdu) = 0;
};

/// This interface notifies to lower layers the generation of new PDUs in the transmitting side of a SDAP entity.
class sdap_pdu_tx_notifier
{
public:
  virtual ~sdap_pdu_tx_notifier() = default;

  /// This callback is invoked on each generated PDU.
  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

} // namespace srs_cu_up

} // namespace srsgnb
