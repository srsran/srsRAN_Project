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

#include "math.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/sch_mcs.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Outer-layer Link Adaptation (OLLA) algorithm, which modifies SNR thresholds used in the selection of MCS
/// to achieve a specified target BLER.
///
/// This technique was first proposed by A Sampath, P Sarath Kumar, JM Holtzman, in IEEE Vehicular Technology
/// Conference, vol. 2. On setting reverse link target sir in a cdma system, (1997).
class olla_algorithm
{
public:
  /// \brief Creats an OLLA algorithm instance.
  ///
  /// \param target_bler_ Target BLER to achieve. Values: [0, 0.5).
  /// \param snr_inc_step_db Constant increment for the SNR offset, in decibels (dB).
  /// \param max_snr_offset_db Maximum SNR offset amplitude, in dB.
  olla_algorithm(float target_bler_, float snr_inc_step_db, float max_snr_offset_db) :
    target_bler(target_bler_),
    delta_down(snr_inc_step_db),
    delta_up((1 - target_bler) * snr_inc_step_db / target_bler),
    max_snr_offset(max_snr_offset_db)
  {
    srsran_assert(target_bler > 0 and target_bler < 0.5, "Invalid target BLER");
    // See Francisco Blanquez-Casado, Gerardo Gomez, Maria del Carmen Aguayo-Torres and Jose Tomas Entrambasaguas,
    // "eOLLA: an enhanced outer loop link adaptation for cellular networks", EURASIP Journal on Wireless
    // Communications and Networking, 2016.
    srsran_assert(delta_down < 2 * std::exp(1) * target_bler / 1.03,
                  "Convergence guarantees of the OLLA algorithm have not been met");
    srsran_assert(max_snr_offset_db > 0.0f, "Invalid maximum SNR offset");
  }

  /// \brief Update SNR offset with ACK/CRC feedback.
  float update(bool ack, sch_mcs_index used_mcs, interval<sch_mcs_index, true> mcs_boundaries)
  {
    // The OLLA algorithm does not account for the saturation points of the MCS. For instance, if we applied the OLLA
    // formula when MCS=28, and average BLER=0, the SNR offset would increase without bounds. As a workaround, we
    // forbid increases in the SNR offset for MCS=maxMCS, and decreases in the SNR offset when MCS=minMCS.
    float eff_delta_down = used_mcs >= mcs_boundaries.stop() ? 0 : delta_down;
    float eff_delta_up   = used_mcs <= mcs_boundaries.start() ? 0 : delta_up;
    current_offset += ack ? eff_delta_down : -eff_delta_up;
    current_offset = std::max(std::min(current_offset, max_snr_offset), -max_snr_offset);
    return current_offset;
  }

  /// \brief Offset to apply to SNR estimate.
  float offset_db() const { return current_offset; }

private:
  /// Target BLER to achieve. Values: [0, 0.5).
  float target_bler;
  /// Delta down and up variables of the OLLA algorithm.
  float delta_down, delta_up;
  /// Maximum SNR offset amplitude, in dB.
  float max_snr_offset;

  /// Current SNR offset in dB, to apply on top of the estimated SNR.
  float current_offset = 0;
};

} // namespace srsran