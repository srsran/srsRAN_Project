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

#include "srsran/phy/constants.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
#include <unordered_map>

namespace srsran {

/// Implements a transform precoder based on the DFT algorithm.
class transform_precoder_dft_impl : public transform_precoder
{
public:
  /// Collection of DFT processors type.
  using collection_dft_processors = std::unordered_map<unsigned, std::unique_ptr<dft_processor>>;

  /// Implementation.
  transform_precoder_dft_impl(collection_dft_processors dft_processors_) : dft_processors(std::move(dft_processors_))
  {
#ifdef ASSERTS_ENABLED
    for (const auto& entry : dft_processors) {
      unsigned M_rb     = entry.first;
      unsigned M_sc     = M_rb * NRE;
      unsigned dft_size = entry.second->get_size();
      srsran_assert(
          is_transform_precoding_nof_prb_valid(M_rb), "Invalid number of resource elements (i.e., {}).", M_rb);
      srsran_assert(M_sc == dft_size,
                    "The DFT size (i.e., {}) is not consistent with the number of subcarriers (i.e., {}).",
                    dft_size,
                    M_sc);
      srsran_assert(entry.second->get_direction() == dft_processor::direction::INVERSE,
                    "Invalid DFT direction (i.e., {}).",
                    dft_processor::direction_to_string(entry.second->get_direction()));
    }
#endif // ASSERTS_ENABLED
  }

  // See interface for documentation.
  void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in) override;

private:
  /// Collection of valid DFT processors.
  collection_dft_processors dft_processors;
};

} // namespace srsran
