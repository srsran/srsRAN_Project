/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/upper/channel_processors/prach_generator.h"
#include "srsgnb/ran/prach/prach_constants.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// \brief On-demand PRACH time-domain signal generator.
///
/// It generates PRACH time-domain signals on demand instead of generating and storing. It minimizes memory footprint at
/// the cost of longer processing time.
class prach_generator_impl : public prach_generator
{
private:
  /// PRACH long sequence length.
  static constexpr unsigned LONG = prach_constants::LONG_SEQUENCE_LENGTH;
  /// PRACH short sequence length.
  static constexpr unsigned SHORT = prach_constants::SHORT_SEQUENCE_LENGTH;

  /// DFT for long frequency-domain sequence generation.
  std::unique_ptr<dft_processor> dft_long;
  /// DFT for short frequency-domain sequence generation.
  std::unique_ptr<dft_processor> dft_short;

  /// Calculates sequence number \f$u\f$ as per TS38.211 Table 6.3.3.1-3.
  static unsigned get_sequence_number_long(unsigned root_sequence_index);

  /// \brief Generates the \f$y_{u,v}\f$ sequence.
  /// \param[in] u   Sequence number.
  /// \param[in] C_v Sequence shift.
  /// \return A read-only view of the generated sequence.
  span<const cf_t> generate_y_u_v_long(unsigned u, unsigned C_v);

public:
  /// \brief PRACH generator constructor.
  ///
  /// The PRACH generator depends on the DFT to generate the time-domain signals.
  ///
  /// \param dft_l839_ DFT processor for generating long sequences.
  /// \param dft_l139_ DFT processor for generating short sequences.
  prach_generator_impl(std::unique_ptr<dft_processor> dft_l839_, std::unique_ptr<dft_processor> dft_l139_) :
    dft_long(std::move(dft_l839_)), dft_short(std::move(dft_l139_))
  {
    report_fatal_error_if_not(dft_long, "Invalid L839 DFT pointer");
    report_fatal_error_if_not(dft_long->get_direction() == dft_processor::direction::DIRECT,
                              "Invalid L839 DFT direction");
    report_fatal_error_if_not(dft_long->get_size() == LONG, "Invalid L839 DFT size");

    report_fatal_error_if_not(dft_short, "Invalid L139 DFT pointer");
    report_fatal_error_if_not(dft_short->get_direction() == dft_processor::direction::DIRECT,
                              "Invalid L139 DFT direction");
    report_fatal_error_if_not(dft_short->get_size() == SHORT, "Invalid L139 DFT size");
  }

  // See interface for documentation.
  span<const cf_t> generate(const configuration& config) override;
};

} // namespace srsgnb
