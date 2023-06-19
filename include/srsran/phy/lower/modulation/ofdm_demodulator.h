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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

class resource_grid_writer;

/// Setup configuration parameters.
struct ofdm_demodulator_configuration {
  /// Indicates the numerology of the OFDM demodulator.
  unsigned numerology;
  /// Indicates the resource grid bandwidth in resource blocks.
  unsigned bw_rb;
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Number of samples to advance the DFT window. The demodulator compensates for the phase shift internally.
  unsigned nof_samples_window_offset;
  /// Scaling factor at the DFT output.
  float scale;
  /// Indicates the center frequency of the carrier in Hz.
  double center_freq_hz;
};

/// \brief Describes an OFDM demodulator that demodulates at symbol granularity.
/// \remark Performs OFDM demodulation as per TS 38.211 section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to demodulation, it applies phase compensation as per TS 38.211 section 5.4 Modulation and
/// upconversion
class ofdm_symbol_demodulator
{
public:
  /// Default destructor.
  virtual ~ofdm_symbol_demodulator() = default;

  /// \brief Gets a symbol size including cyclic prefix.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples for the given symbol index.
  virtual unsigned get_symbol_size(unsigned symbol_index) const = 0;

  /// \brief Demodulates an OFDM signal with symbol granularity.
  /// \param[out] grid Provides the output as frequency-domain signal corresponding to one slot.
  /// \param[in] input Provides the time domain modulated signal.
  /// \param[in] port_index Indicates the port index to demodulate.
  /// \param[in] symbol_index Symbol index within the subframe to demodulate.
  /// \note The input size must be equal to the the symbol size including the cyclic prefix.
  /// \note The output size must be consistent with the configured bandwidth.
  virtual void
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned symbol_index) = 0;
};

/// \brief Describes an OFDM demodulator with slot granularity.
/// \remark Performs OFDM demodulation as per TS 38.211 section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to demodulation, it applies phase compensation as per TS 38.211 section 5.4 Modulation and
/// upconversion.
class ofdm_slot_demodulator
{
public:
  /// Default destructor.
  virtual ~ofdm_slot_demodulator() = default;

  /// \brief Gets a slot size.
  /// \param[in] slot_index Indicates the slot index within the subframe.
  /// \return The number of samples for the given slot index.
  virtual unsigned get_slot_size(unsigned slot_index) const = 0;

  /// \brief Demodulates an OFDM signal with slot granularity.
  /// \param[out] grid Provides the output as frequency-domain signal corresponding to one slot.
  /// \param[in] input Provides the time domain modulated signal source.
  /// \param[in] port_index Indicates the port index to demodulate.
  /// \param[in] slot_index Slot index within the subframe to demodulate.
  /// \note The input size must be must be equal to the slot size including the cyclic prefixes.
  /// \note The output size consistent with the configured bandwidth.
  virtual void
  demodulate(resource_grid_writer& grid, span<const cf_t> input, unsigned port_index, unsigned slot_index) = 0;
};

} // namespace srsran
