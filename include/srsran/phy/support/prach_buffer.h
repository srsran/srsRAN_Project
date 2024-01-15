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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief PRACH buffer interface.
///
/// Provides access to frequency-domain PRACH sequences.
///
/// A buffer for storing long PRACH sequences (\f$L_{RA}=839\f$), for a maximum of 4 OFDM symbols, can be created with
/// the factory function create_prach_buffer_long(). A buffer for storing short PRACH sequences (\f$L_{RA}=129\f$), for
/// a maximum of 12 OFDM symbols, can be created with the factory function create_prach_buffer_short().
class prach_buffer
{
public:
  /// Default destructor.
  virtual ~prach_buffer() = default;

  /// Gets the maximum number of ports.
  virtual unsigned get_max_nof_ports() const = 0;

  /// Gets the maximum number of time domain occasions.
  virtual unsigned get_max_nof_td_occasions() const = 0;

  /// Gets the maximum number of frequency domain occasions.
  virtual unsigned get_max_nof_fd_occasions() const = 0;

  /// Gets the maximum number of symbols.
  virtual unsigned get_max_nof_symbols() const = 0;

  /// Gets the sequence length.
  virtual unsigned get_sequence_length() const = 0;

  /// \brief Gets a read-write PRACH symbol for a given port, occasion and symbol.
  ///
  /// \param[in] i_port        Port identifier.
  /// \param[in] i_td_occasion Time-domain occasion.
  /// \param[in] i_fd_occasion Frequency-domain occasion.
  /// \param[in] i_symbol      Symbol index within the occasion.
  /// \return A read-write view of a PRACH OFDM symbol.
  virtual span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) = 0;

  /// \brief Gets a read-only PRACH symbol for a given port, occasion and symbol.
  ///
  /// \param[in] i_port        Port identifier.
  /// \param[in] i_td_occasion Time-domain occasion.
  /// \param[in] i_fd_occasion Frequency-domain occasion.
  /// \param[in] i_symbol      Symbol index within the occasion.
  /// \return A read-only view of a PRACH OFDM symbol.
  virtual span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const = 0;
};

} // namespace srsran
