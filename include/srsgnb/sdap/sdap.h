/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"

namespace srsran {

namespace srs_cu_up {

// :TODO: These are the main input/output interfaces to pass data traffic between layers. As the implementation
// progresses on we'll add additional files that include new interfaces for configuration and other responsibilities.
// Please note these interfaces represent a single bearer or SDAP entity.

/// This interface represents the entry point of the receiving side of a SDAP entity.
class sdap_rx_pdu_handler
{
public:
  virtual ~sdap_rx_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_pdu(byte_buffer pdu) = 0;
};

/// This interface notifies to upper layers the reception of new SDUs in the receiving side of a SDAP entity.
class sdap_rx_sdu_notifier
{
public:
  virtual ~sdap_rx_sdu_notifier() = default;

  /// This callback is invoked on each generated SDU.
  virtual void on_new_sdu(byte_buffer sdu) = 0;
};

/// This interface notifies to lower layers the generation of new PDUs in the transmitting side of a SDAP entity.
class sdap_tx_pdu_notifier
{
public:
  virtual ~sdap_tx_pdu_notifier() = default;

  /// This callback is invoked on each generated PDU.
  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

/// This interface represents the entry point of the transmitting side of a SDAP entity.
class sdap_tx_sdu_handler
{
public:
  virtual ~sdap_tx_sdu_handler() = default;

  /// Handle the incoming SDU.
  virtual void handle_sdu(byte_buffer sdu) = 0;
};

/// Interface for the SDAP entity.
/// Provides getters for the RX and TX parts of the SDAP entity.
class sdap_entity
{
public:
  sdap_entity()          = default;
  virtual ~sdap_entity() = default;

  virtual sdap_rx_pdu_handler& get_sdap_rx_pdu_handler() = 0;
  virtual sdap_tx_sdu_handler& get_sdap_tx_sdu_handler() = 0;
};

} // namespace srs_cu_up

} // namespace srsran
