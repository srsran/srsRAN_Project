/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/slot_symbol_point.h"

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul interface to handle OTA symbol-based timing events.
class ota_symbol_handler
{
public:
  /// Default destructor.
  virtual ~ota_symbol_handler() = default;

  /// \brief Callback function called on a new OFDM symbol timing event.
  /// \param[in] symbol_point Current slot and symbol point.
  virtual void handle_new_ota_symbol(slot_symbol_point symbol_point) = 0;
};

} // namespace ofh
} // namespace srsran
