/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/lower_phy_baseband_metrics.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Physical downlink modulator notifier interface.
class pdxch_processor_modulator_notifier
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_modulator_notifier() = default;

  /// \brief Notifies the completion of the OFDM modulation for a given slot.
  /// \param[in] result  Baseband modulation results.
  /// \param[in] context Modulated resource grid context.
  virtual void on_modulation_completion(pdxch_processor_baseband::slot_result result,
                                        resource_grid_context                 context) = 0;
};

} // namespace srsran
