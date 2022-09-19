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

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;
class uplink_request_processor;
class upper_phy_rx_results_notifier;
class upper_phy_rx_symbol_handler;
class upper_phy_timing_handler;
class upper_phy_timing_notifier;
class uplink_slot_pdu_repository;

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

  /// \brief Returns the uplink resource grid pool of this upper PHY.
  virtual resource_grid_pool& get_uplink_resource_grid_pool() = 0;

  /// \brief Returns the uplink request processor of this upper PHY.
  virtual uplink_request_processor& get_uplink_request_processor() = 0;

  /// \brief Returns the uplink slot PDU repository of this upper PHY.
  virtual uplink_slot_pdu_repository& get_uplink_slot_pdu_repository() = 0;

  /// \brief Sets the upper PHY timing notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_timing_notifier(upper_phy_timing_notifier& notifier) = 0;

  /// \brief \brief Sets the receive result notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_results_notifier(upper_phy_rx_results_notifier& notifier) = 0;
};

} // namespace srsgnb
