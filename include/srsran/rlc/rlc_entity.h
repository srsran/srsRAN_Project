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

#include "srsran/rlc/rlc_metrics.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"

namespace srsran {

/// Class used to interface with an RLC entity.
/// It will contain getters for the TX and RX entities interfaces.
class rlc_entity
{
public:
  rlc_entity()                              = default;
  virtual ~rlc_entity()                     = default;
  rlc_entity(const rlc_entity&)             = delete;
  rlc_entity& operator=(const rlc_entity&)  = delete;
  rlc_entity(const rlc_entity&&)            = delete;
  rlc_entity& operator=(const rlc_entity&&) = delete;

  virtual rlc_tx_upper_layer_data_interface* get_tx_upper_layer_data_interface() = 0;
  virtual rlc_tx_lower_layer_interface*      get_tx_lower_layer_interface()      = 0;
  virtual rlc_rx_lower_layer_interface*      get_rx_lower_layer_interface()      = 0;
  virtual rlc_metrics                        get_metrics()                       = 0;
};

} // namespace srsran
