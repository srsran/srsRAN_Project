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

#include "srsgnb/phy/upper/channel_processors/prach_generator.h"
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

/// \brief On-demand PRACH time-domain signal generator.
///
/// It generates PRACH frequency-domain signals on demand instead of generating and storing them off-line. It minimizes
/// memory footprint at the cost of longer processing time.
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

  /// \brief Generates the sequence \f$y_{u,v}\f$.
  /// \param[in] u   Sequence number.
  /// \param[in] C_v Sequence shift.
  /// \return A read-only view of the generated sequence.
  span<const cf_t> generate_y_u_v_long(unsigned u, unsigned C_v);

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

} // namespace srsgnb
