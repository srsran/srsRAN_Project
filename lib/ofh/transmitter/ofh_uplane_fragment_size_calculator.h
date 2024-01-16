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

  /// \brief Calculates number of segments required to codify one resource grid symbol given the frame size.
  ///
  /// \param[in] frame_size   Frame size.
  /// \param[in] nof_prbs     Number of PRBs in a resource grid.
  /// \param[in] comp_params  Compression parameters for the I and Q samples.
  /// \param[in] headers_size Size of the headers stored in a frame alongside the I and Q samples.
  /// \return Number of segments.
  static unsigned calculate_nof_segments(units::bytes                 frame_size,
                                         unsigned                     nof_prbs,
                                         const ru_compression_params& comp_params,
                                         units::bytes                 headers_size);

private:
  const unsigned     start_prb;
  const unsigned     nof_prbs;
  const units::bytes prb_size;
  unsigned           next_fragment_start_prb_index;
};

} // namespace ofh
} // namespace srsran
