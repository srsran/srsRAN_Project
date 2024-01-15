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
