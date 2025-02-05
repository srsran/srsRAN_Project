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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/math/math_utils.h"
#include <unordered_map>

namespace srsran {

/// DFT-based implementation of the time alignment estimator.
class time_alignment_estimator_dft_impl : public time_alignment_estimator
{
public:
  /// Maximum number of PRB.
  static constexpr unsigned max_nof_re = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB;

  /// Minimum DFT size to satisfy the minimum TA measurement.
  static const unsigned min_dft_size;

  /// Maximum DFT size to fit all the channel bandwidth.
  static const unsigned max_dft_size;

  /// Collection of DFT processors type.
  using collection_dft_processors = std::unordered_map<unsigned, std::unique_ptr<dft_processor>>;

  /// Required DFT direction.
  static constexpr dft_processor::direction dft_direction = dft_processor::direction::INVERSE;

  /// \brief Creates the time alignment estimator instance.
  ///
  /// \remark An assertion is triggered if the DFT processor direction or the DFT size are not as expected.
  time_alignment_estimator_dft_impl(collection_dft_processors dft_processors_);

  // See interface for documentation.
  time_alignment_measurement estimate(span<const cf_t>                symbols,
                                      bounded_bitset<max_nof_symbols> mask,
                                      subcarrier_spacing              scs,
                                      double                          max_ta) override;

  // See interface for documentation.
  time_alignment_measurement estimate(const re_buffer_reader<cf_t>&   symbols,
                                      bounded_bitset<max_nof_symbols> mask,
                                      subcarrier_spacing              scs,
                                      double                          max_ta) override;

  // See interface for documentation.
  time_alignment_measurement
  estimate(span<const cf_t> symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override;

  // See interface for documentation.
  time_alignment_measurement
  estimate(const re_buffer_reader<cf_t>& symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override;

private:
  /// DFT processors.
  collection_dft_processors dft_processors;

  /// Get the DFT that best suits the maximum number of resource elements.
  dft_processor& get_idft(unsigned nof_required_re);

  /// Estimates the TA assuming the complex symbols are already in the DFT input.
  static time_alignment_measurement
  estimate_ta_correlation(span<const float> correlation, unsigned stride, subcarrier_spacing scs, double max_ta);

  /// Buffer for storing the IDFT magnitude square.
  std::vector<float> idft_abs2;
}; // namespace srsran

} // namespace srsran
