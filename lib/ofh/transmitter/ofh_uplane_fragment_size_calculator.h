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
                                      const ru_compression_params& compr_params);

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
  /// \param[in] compr_params  Compression parameters for the I and Q samples.
  /// \param[in] headers_size Size of the headers stored in a frame alongside the I and Q samples.
  /// \return Number of segments.
  static unsigned calculate_nof_segments(units::bytes                 frame_size,
                                         unsigned                     nof_prbs,
                                         const ru_compression_params& compr_params,
                                         units::bytes                 headers_size);

private:
  const unsigned     start_prb;
  const unsigned     nof_prbs;
  const units::bytes prb_size;
  unsigned           next_fragment_start_prb_index;
};

} // namespace ofh
} // namespace srsran
