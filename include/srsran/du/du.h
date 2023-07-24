/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
