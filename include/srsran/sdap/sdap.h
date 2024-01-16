/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/sdap/sdap_config.h"

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
  virtual void on_new_sdu(byte_buffer sdu, qos_flow_id_t qfi) = 0;
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
  virtual void handle_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) = 0;
};

/// Interface for the SDAP entity.
/// Provides getters for the RX and TX parts of the SDAP entity.
class sdap_entity
{
public:
  sdap_entity()          = default;
  virtual ~sdap_entity() = default;

  virtual sdap_rx_pdu_handler& get_sdap_rx_pdu_handler(drb_id_t drb_id) = 0;
  virtual sdap_tx_sdu_handler& get_sdap_tx_sdu_handler()                = 0;

  virtual bool is_mapped(qos_flow_id_t qfi) = 0;
  virtual void
  add_mapping(qos_flow_id_t qfi, drb_id_t drb_id, sdap_config sdap_cfg, sdap_tx_pdu_notifier& tx_pdu_notifier) = 0;
  virtual void remove_mapping(drb_id_t drb_id)                                                                 = 0;
};

} // namespace srs_cu_up

} // namespace srsran
