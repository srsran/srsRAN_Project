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

#include "srsran/phy/upper/channel_processors/prach_generator.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/srsvec/aligned_vec.h"

namespace srsran {

/// \brief On-demand PRACH time-domain signal generator.
///
/// It generates PRACH frequency-domain signals on demand instead of generating and storing them off-line. It minimizes
/// memory footprint at the cost of longer processing time.
///
/// More details about the generation algorithm can be found in
/// > D. Gregoratti, X. Arteaga, and J. Broquetas, "Mathematical Properties of the Zadoff-Chu Sequences,"
/// > online: https://arxiv.org/abs/2311.01035
class prach_generator_impl : public prach_generator
{
private:
  /// PRACH long sequence length.
  static constexpr unsigned LONG = prach_constants::LONG_SEQUENCE_LENGTH;
  /// PRACH short sequence length.
  static constexpr unsigned SHORT = prach_constants::SHORT_SEQUENCE_LENGTH;

  /// Temporary sequence.
  srsvec::aligned_vec<cf_t> sequence;

  /// Calculates sequence number \f$u\f$ as per TS38.211 Table 6.3.3.1-3.
  static unsigned get_sequence_number_long(unsigned root_sequence_index);

  /// Calculates sequence number \f$u\f$ as per TS38.211 Table 6.3.3.1-4.
  static unsigned get_sequence_number_short(unsigned root_sequence_index);

  /// \brief Generates the sequence \f$y_{u,v}\f$ for long preambles.
  /// \param[in] u   Sequence number.
  /// \param[in] C_v Sequence shift.
  /// \return A read-only view of the generated sequence.
  span<const cf_t> generate_y_u_v_long(unsigned u, unsigned C_v);

  /// \brief Generates the sequence \f$y_{u,v}\f$ for short preambles.
  /// \param[in] u   Sequence number.
  /// \param[in] C_v Sequence shift.
  /// \return A read-only view of the generated sequence.
  span<const cf_t> generate_y_u_v_short(unsigned u, unsigned C_v);

public:
  /// \brief Constructor - Acquires ownership of the internal components.
  ///
  /// The PRACH generator depends on the DFT to generate the frequency-domain signals.
  ///
  /// \param dft_long_ DFT processor for generating long sequences.
  /// \param dft_short_ DFT processor for generating short sequences.
  prach_generator_impl() : sequence(LONG)
  {
    // Do nothing.
  }

  // See interface for documentation.
  span<const cf_t> generate(const configuration& config) override;
};

} // namespace srsran
