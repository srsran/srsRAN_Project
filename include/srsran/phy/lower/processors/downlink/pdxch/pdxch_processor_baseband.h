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

#include "srsran/ran/slot_point.h"

namespace srsran {

class baseband_gateway_buffer_writer;
struct lower_phy_rx_symbol_context;

/// \brief Lower physical layer PDxCH processor - Baseband interface.
///
/// Processes baseband samples with OFDM symbol granularity. The OFDM symbol size is inferred from the slot numerology.
class pdxch_processor_baseband
{
public:
  /// Describes the context of a newly received symbol.
  struct symbol_context {
    /// Slot context.
    slot_point slot;
    /// Radio sector identifier.
    unsigned sector;
    /// The processed symbol index within the slot.
    unsigned symbol;
  };

  /// Default destructor.
  virtual ~pdxch_processor_baseband() = default;

  /// \brief Processes a baseband OFDM symbol.
  ///
  /// Modulates the OFDM symbol indicated by \c context and stores the baseband samples in \c samples. If the
  /// resource grid is not available at the time of calling this method, the symbol processing is skipped, and the
  /// sample buffer is not modified.
  ///
  /// \param[out] samples Baseband samples to process.
  /// \param[in] context OFDM Symbol context.
  /// \return \c true if the symbol has been processed, \c false otherwise.
  virtual bool process_symbol(baseband_gateway_buffer_writer& samples, const symbol_context& context) = 0;
};

} // namespace srsran
