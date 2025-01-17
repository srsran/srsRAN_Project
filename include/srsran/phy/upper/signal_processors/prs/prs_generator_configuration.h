/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/prs/prs.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Positioning Reference Signals (PRS) transmission parameters.
///
/// \remark Not all combinations of comb size and time domain duration are valid, see \ref
/// prs_valid_num_symbols_and_comb_size for more information.
struct prs_generator_configuration {
  /// Slot and subcarrier spacing.
  slot_point slot;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// \brief Sequence identifier.
  ///
  /// Parameter \f$n_{ID,seq}^{PRS}\f$ in TS38.211 Section 7.4.1.7.2. The range is {0, ..., 4095}. It is given by the
  /// higher layer parameter \e dl-PRS-SequenceID.
  ///
  uint16_t n_id_prs;
  /// \brief Transmission comb size.
  ///
  /// Parameter \f$K_{comb}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-CombSizeN.
  prs_comb_size comb_size;
  /// \brief Transmission comb offset.
  ///
  /// Parameter \f$k_{offset}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-CombSizeN-AndReOffset
  uint8_t comb_offset;
  /// \brief Transmission time domain duration.
  ///
  /// Parameter \f$L_{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-NumSymbols.
  prs_num_symbols duration;
  /// \brief First symbol index of the transmission.
  ///
  /// Parameter \f$l_{start}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-ResourceSymbolOffset.
  uint8_t start_symbol;
  /// \brief Physical resource block allocation start PRB.
  ///
  /// It is given by the higher layer parameter \e dl-PRS-StartPRB. The range is {0, ..., 2176}.
  ///
  /// It expresses the number of PRB between the PointA given by the higher layer parameter \e dl-PRS-PointA and the
  /// lowest PRB of the PRS transmission.
  uint16_t prb_start;
  /// \brief Physical resource block allocation expressed as a start and number of physical resource blocks.
  ///
  /// The start position is relative to the lowest resource grid physical resource block. Set the start position equal
  /// to \c prb_start, if the higher layer parameter \e dl-PRS-PointA matches with the lowest subcarrier of the resource
  /// grid.
  ///
  /// The number of PRB is derived from the higher layer parameter \e dl-PRS-ResourceBandwidth. The range is
  /// {24, ..., 276} in steps of 4.
  interval<uint16_t> freq_alloc;
  /// Ratio of PRS data EPRE to SSS EPRE in decibels.
  float power_offset_dB;
  /// Precoding configuration.
  precoding_configuration precoding;
};

} // namespace srsran
