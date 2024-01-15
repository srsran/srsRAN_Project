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

namespace srsran {

class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class upper_phy_rx_results_notifier;
class upper_phy_rx_symbol_handler;
class upper_phy_timing_handler;
class upper_phy_timing_notifier;
class uplink_slot_pdu_repository;
class tx_buffer_pool;

/// \brief Upper PHY interface.
///
/// This interface describes the upper PHY layer giving access to its gateways and event notifiers.
///
/// The downlink part of the upper PHY processes different PDU types such as PDCCH, PDSCH, NZI-CSI-RS and SSB PDUs and
/// then sends the resulting resource grid through the configured \c resource_grid_gateway.
///
/// The uplink part of the upper PHY notifies about the new incoming symbols, processing them to generate the uplink
/// PDUs such as PUSCH or PUCCH. It also notifies when a new slot starts.
class upper_phy
{
public:
  /// Default destructor.
  virtual ~upper_phy() = default;

  /// \brief Returns a reference to the receive symbol handler of this upper PHY.
  virtual upper_phy_rx_symbol_handler& get_rx_symbol_handler() = 0;

  /// \brief Returns a reference to the timing handler of this upper PHY.
  virtual upper_phy_timing_handler& get_timing_handler() = 0;

  /// \brief Returns the downlink processor pool of this upper PHY.
  virtual downlink_processor_pool& get_downlink_processor_pool() = 0;

  /// \brief Returns the downlink resource grid pool of this upper PHY.
  virtual resource_grid_pool& get_downlink_resource_grid_pool() = 0;

  /// \brief Returns the transmit buffer pool.
  virtual tx_buffer_pool& get_tx_buffer_pool() = 0;

  /// \brief Returns the uplink resource grid pool of this upper PHY.
  virtual resource_grid_pool& get_uplink_resource_grid_pool() = 0;

  /// \brief Returns the uplink request processor of this upper PHY.
  virtual uplink_request_processor& get_uplink_request_processor() = 0;

  /// \brief Returns the uplink slot PDU repository of this upper PHY.
  virtual uplink_slot_pdu_repository& get_uplink_slot_pdu_repository() = 0;

  /// \brief Returns the downlink PDU validator of this upper PHY.
  virtual const downlink_pdu_validator& get_downlink_pdu_validator() const = 0;

  /// \brief Returns the uplink PDU validator of this upper PHY.
  virtual const uplink_pdu_validator& get_uplink_pdu_validator() const = 0;

  /// \brief Sets the upper PHY timing notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_timing_notifier(upper_phy_timing_notifier& notifier) = 0;

  /// \brief \brief Sets the receive result notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_rx_results_notifier(upper_phy_rx_results_notifier& notifier) = 0;
};

} // namespace srsran
