/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

class f1ap_message_handler;
class upper_phy_error_handler;
class upper_phy_rx_symbol_handler;
class upper_phy_timing_handler;

class du
{
public:
  virtual ~du() = default;

  /// Starts the DU.
  virtual void start() = 0;

  /// Stops the DU.
  virtual void stop() = 0;

  /// Returns the handler for F1AP Rx PDUs coming from the CU-CP.
  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;

  /// Returns a reference to the error handler of the DU.
  virtual upper_phy_error_handler& get_error_handler() = 0;

  /// Returns handler in charge of processing uplink OFDM symbols.
  virtual upper_phy_rx_symbol_handler& get_rx_symbol_handler() = 0;

  /// Returns a reference to the timing handler of the DU.
  virtual upper_phy_timing_handler& get_timing_handler() = 0;
};

} // namespace srsran
