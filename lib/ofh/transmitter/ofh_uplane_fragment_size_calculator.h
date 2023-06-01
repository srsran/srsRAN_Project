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

#include "srsran/ofh/compression/compression_params.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul fragment size calculator.
///
/// This class calculates the fragments of a configured PRB range, based on the given frame size and compression
/// parameters. This class assumes one section per fragment.
class ofh_uplane_fragment_size_calculator
{
public:
  ofh_uplane_fragment_size_calculator(unsigned                     start_prb_,
                                      unsigned                     nof_prbs_,
                                      const ru_compression_params& comp_params);

  /// \brief Calculates the fragment size of the configured PRBs that will fit in the given frame size.
  ///
  /// \param[out] fragment_start_prb Start PRB for frame.
  /// \param[out] fragment_nof_prbs Number of PRBs that fit in the frame.
  /// \param[in] frame_size Frame size.
  /// \return True if it is the last fragment, otherwise false.
  bool calculate_fragment_size(unsigned& fragment_start_prb, unsigned& fragment_nof_prbs, unsigned frame_size);

private:
  const unsigned     start_prb;
  const unsigned     nof_prbs;
  const units::bytes prb_size;
  unsigned           next_fragment_start_prb_index;
};

} // namespace ofh
} // namespace srsran
