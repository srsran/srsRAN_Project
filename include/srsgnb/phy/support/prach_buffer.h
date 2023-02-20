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

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsran {

/// \brief PRACH buffer interface.
///
/// Provides access to frequency-domain PRACH sequences.
///
/// A buffer for storing long PRACH sequences (\f$L_{RA}=839\f$), for a maximum of 4 OFDM symbols, can be created with
/// the factory function create_prach_buffer_long(). A buffer for storing short PRACH sequences (\f$L_{RA}=129\f$), for
/// a maximum of 12 OFDM symbols, can be created with the factory function create_prach_buffer_short().
///
/// \note In future versions of the software, this interface might be upgraded to support multiple PRACH occasions, both
/// in time and frequency domains.
class prach_buffer
{
public:
  /// Default destructor.
  virtual ~prach_buffer() = default;

  /// Gets the sequence length.
  virtual unsigned get_sequence_length() const = 0;

  /// Gets the maximum number of symbols.
  virtual unsigned get_max_nof_symbols() const = 0;

  /// Gets a read-write view of a PRACH OFDM symbol.
  virtual span<cf_t> get_symbol(unsigned symbol_index) = 0;

  /// Gets a read-only view of a PRACH OFDM symbol.
  virtual span<const cf_t> get_symbol(unsigned symbol_index) const = 0;
};

} // namespace srsran
