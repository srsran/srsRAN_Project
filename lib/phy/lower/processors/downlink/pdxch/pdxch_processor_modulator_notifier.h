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
