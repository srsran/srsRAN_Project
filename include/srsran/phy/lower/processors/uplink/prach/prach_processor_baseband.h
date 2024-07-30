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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class baseband_gateway_buffer_reader;

/// \brief Lower physical layer PRACH processor - Baseband interface.
///
/// Processes baseband samples with OFDM symbol granularity. The OFDM symbol size is inferred from the slot numerology.
class prach_processor_baseband
{
public:
  /// Default destructor.
  virtual ~prach_processor_baseband() = default;

  /// Collects the parameters that describe a symbol context.
  struct symbol_context {
    /// Current slot.
    slot_point slot;
    /// Current symbol index within the slot.
    unsigned symbol;
    /// Sector identifier.
    unsigned sector;
  };

  /// \brief Processes a baseband OFDM symbol.
  ///
  /// \param[in] samples Baseband samples to process.
  /// \param[in] context OFDM Symbol context.
  virtual void process_symbol(const baseband_gateway_buffer_reader& samples, const symbol_context& context) = 0;
};

} // namespace srsran
