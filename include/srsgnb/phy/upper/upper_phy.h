/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_H

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;
class upper_phy_rx_symbol_handler;
class upper_phy_timing_handler;
class upper_phy_timing_notifier;

/// \brief This interface describes the upper PHY layer giving access to its gateways and event notifications.
///
/// The downlink part of the upper PHY processes different PDU types such as PDCCH, PDSCH, NZI-CSI-RS and SSB PDUs and
/// then sends the resulting resource grid through the configured \c resource_grid_gateway.
///
/// The uplink part of the upper PHY notifies about the new incoming symbols, processing them to generate the uplink
/// PDUs such as PUSCH or PUCCH. It also notifies when a new slot starts.
class upper_phy
{
public:
  /// Default destructor used for destroying implementations from a pointer to the interface.
  virtual ~upper_phy() = default;

  /// \brief Returns a reference to the reception symbol handler of this upper PHY.
  virtual upper_phy_rx_symbol_handler& get_upper_phy_rx_symbol_handler() = 0;

  /// \brief Returns a reference to the timing handler of this upper PHY.
  virtual upper_phy_timing_handler& get_upper_phy_timing_handler() = 0;

  /// \brief Returns the downlink processor pool of this upper PHY.
  virtual downlink_processor_pool& get_downlink_processor_pool() = 0;

  /// \brief Returns the downlink resource grid pool of this upper PHY.
  virtual resource_grid_pool& get_downlink_resource_grid_pool() = 0;

  /// \brief Sets the upper_phy_timing_notifier for this upper PHY to the given one.
  ///
  /// \param notifier Notifier to be set.
  virtual void set_upper_phy_notifier(upper_phy_timing_notifier& notifier) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_UPPER_PHY_H
