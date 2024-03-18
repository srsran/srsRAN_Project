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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"

namespace srsran {

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
  time_alignment_measurement
  estimate(span<const cf_t> symbols, bounded_bitset<max_nof_symbols> mask, subcarrier_spacing scs) override;

private:
  /// DFT processor for converting frequency domain to time domain.
  std::unique_ptr<dft_processor> idft;
};

} // namespace srsran
