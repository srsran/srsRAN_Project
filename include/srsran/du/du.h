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

#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsran/phy/upper/upper_phy_timing_handler.h"

namespace srsran {

class du
{
public:
  virtual ~du() = default;

  /// \brief Start the DU.
  virtual void start() = 0;

  /// \brief Stop the DU.
  virtual void stop() = 0;

  /// \brief Get the handler for F1AP Rx PDUs coming from the CU-CP.
  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;

  /// \brief Gets handler in charge of processing uplink OFDM symbols.
  virtual upper_phy_rx_symbol_handler& get_rx_symbol_handler() = 0;

  /// \brief Returns a reference to the timing handler of the DU.
  virtual upper_phy_timing_handler& get_timing_handler() = 0;
};

} // namespace srsran
