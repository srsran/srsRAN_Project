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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"

namespace srsran {

/// DFT-based implementation of the time alignment estimator.
class time_alignment_estimator_dft_impl : public time_alignment_estimator
{
public:
  /// Required DFT size.
  static constexpr unsigned dft_size = 4096;
  /// Required DFT direction.
  static constexpr dft_processor::direction dft_direction = dft_processor::direction::INVERSE;

  /// \brief Creates the time alignment estimator instance.
  ///
  /// \remark An assertion is triggered if the DFT processor direction or the DFT size are not as expected.
  time_alignment_estimator_dft_impl(std::unique_ptr<dft_processor> idft_) : idft(std::move(idft_))
  {
    srsran_assert(idft, "Invalid DFT processor.");
    srsran_assert(idft->get_direction() == dft_processor::direction::INVERSE, "Invalid DFT processor direction.");
    srsran_assert(idft->get_size() == dft_size, "Invalid DFT processor size (i.e., {}).", idft->get_size());
  }

  // See interface for documentation.
  time_alignment_measurement estimate(span<const cf_t>                symbols,
                                      bounded_bitset<max_nof_symbols> mask,
                                      subcarrier_spacing              scs,
                                      double                          max_ta) override;

  // See interface for documentation.
  time_alignment_measurement
  estimate(span<const cf_t> symbols, unsigned stride, subcarrier_spacing scs, double max_ta) override;

private:
  /// DFT processor for converting frequency domain to time domain.
  std::unique_ptr<dft_processor> idft;

  /// Estimates the TA assuming the complex symbols are already in the DFT input.
  time_alignment_measurement estimate(subcarrier_spacing scs, double max_ta);
};

} // namespace srsran
