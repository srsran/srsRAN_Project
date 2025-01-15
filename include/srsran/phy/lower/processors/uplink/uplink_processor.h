/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

class uplink_processor_notifier;
class uplink_processor_baseband;
class prach_processor_request_handler;
class puxch_processor_request_handler;
class prach_processor_notifier;
class puxch_processor_notifier;
class lower_phy_cfo_controller;

/// \brief Uplink processor main interface.
///
/// Provides a single access point to all uplink-related interfaces, that is uplink_processor_notifier,
/// uplink_processor_request_handler and uplink_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the uplink processor is not connected
/// to a notifier before calling get_prach_request_handler(), get_puxch_request_handler() or get_baseband().
class lower_phy_uplink_processor
{
public:
  /// Default destructor.
  virtual ~lower_phy_uplink_processor() = default;

  /// Connects the uplink processor with notifiers.
  virtual void connect(uplink_processor_notifier& notifier,
                       prach_processor_notifier&  prach_notifier,
                       puxch_processor_notifier&  puxch_notifier) = 0;

  /// Gets the PRACH request handler.
  virtual prach_processor_request_handler& get_prach_request_handler() = 0;

  /// Gets the PUxCH request handler.
  virtual puxch_processor_request_handler& get_puxch_request_handler() = 0;

  /// \brief Gets the carrier frequency offset processor interface.
  virtual lower_phy_cfo_controller& get_cfo_handler() = 0;

  /// \brief Gets the uplink processor baseband interface.
  /// \return A reference to the internal uplink processor baseband interface.
  virtual uplink_processor_baseband& get_baseband() = 0;
};

} // namespace srsran