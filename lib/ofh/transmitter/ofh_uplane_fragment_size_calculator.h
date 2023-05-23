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
