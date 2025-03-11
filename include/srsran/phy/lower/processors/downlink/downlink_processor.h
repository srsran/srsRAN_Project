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

class downlink_processor_baseband;
class downlink_processor_notifier;
class downlink_processor_notifier;
class lower_phy_cfo_controller;
class pdxch_processor_notifier;
class pdxch_processor_request_handler;

/// \brief Downlink processor main interface.
///
/// Provides a single access point to all downlink-related interfaces, that is \ref downlink_processor_notifier,
/// \ref downlink_processor_request_handler and \ref downlink_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the downlink processor is not
/// connected to a notifier before calling \ref get_downlink_request_handler() or \ref get_baseband().
class lower_phy_downlink_processor
{
public:
  /// Default destructor.
  virtual ~lower_phy_downlink_processor() = default;

  /// Connects the downlink processor with notifiers.
  virtual void connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier) = 0;

  /// Gets the carrier frequency offset processor interface.
  virtual lower_phy_cfo_controller& get_cfo_handler() = 0;

  /// Gets the downlink processor request handler.
  virtual pdxch_processor_request_handler& get_downlink_request_handler() = 0;

  /// \brief Gets the downlink processor baseband interface.
  /// \return A reference to the internal downlink processor baseband interface.
  virtual downlink_processor_baseband& get_baseband() = 0;
};

} // namespace srsran
