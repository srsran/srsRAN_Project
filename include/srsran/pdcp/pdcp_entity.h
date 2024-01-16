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
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_rx_metrics.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/pdcp/pdcp_tx_metrics.h"
#include "srsran/security/security.h"

namespace srsran {

struct pdcp_metrics_container {
  pdcp_tx_metrics_container tx;
  pdcp_rx_metrics_container rx;
};

/// Interface for the PDCP bearer.
/// Provides getters and setters for the RX and TX parts of the PDCP entity.
class pdcp_entity
{
public:
  pdcp_entity()          = default;
  virtual ~pdcp_entity() = default;

  virtual pdcp_tx_lower_interface&         get_tx_lower_interface()         = 0;
  virtual pdcp_tx_upper_control_interface& get_tx_upper_control_interface() = 0;
  virtual pdcp_tx_upper_data_interface&    get_tx_upper_data_interface()    = 0;
  virtual pdcp_rx_upper_control_interface& get_rx_upper_control_interface() = 0;
  virtual pdcp_rx_lower_interface&         get_rx_lower_interface()         = 0;
  virtual pdcp_metrics_container           get_metrics()                    = 0;
};

} // namespace srsran
