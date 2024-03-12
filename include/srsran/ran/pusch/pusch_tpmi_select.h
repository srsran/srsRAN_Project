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

#include "srsran/adt/interval.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// PUSCH Transmit Precoding Matrix Indication (TPMI) information.
class pusch_tpmi_select_info
{
public:
  struct tpmi_info {
    /// Most suitable Transmit Precoding Matrix Indicator.
    unsigned tpmi;
    /// Average Signal-to-Interference-plus-Noise Ratio (SINR) in decibels.
    float sinr_dB;
  };

  /// Gets the maximum number of layers.
  unsigned get_max_nof_layers() const { return info.size(); }

  /// Determines if the information is valid.
  bool is_valid() const { return !info.empty(); }

  /// \brief Gets the TPMI information for a number of layers.
  ///
  /// \remark An assertion is triggered if the number of layers is out of range.
  const tpmi_info& get_tpmi_select(unsigned nof_layers) const
  {
    interval<unsigned, true> nof_layers_range(1, get_max_nof_layers());
    srsran_assert(nof_layers_range.contains(nof_layers),
                  "The number of layers (i.e., {}) is out-of-range {}",
                  nof_layers,
                  nof_layers_range);

    return info[nof_layers - 1];
  }

  /// Constructs a PUSCH TPMI information from a span.
  pusch_tpmi_select_info(const span<tpmi_info>& info_) : info(info_.begin(), info_.end()) {}

  /// Constructs a PUSCH TPMI information from an initializer list.
  pusch_tpmi_select_info(const std::initializer_list<tpmi_info>& info_) : info(info_.begin(), info_.end()) {}

private:
  /// TPMI information for each number of layers.
  static_vector<tpmi_info, pusch_constants::MAX_NOF_LAYERS> info;
};

/// \brief Selects the Transmit Precoding Matrix Indicator (TPMI) for each possible number of layers supported by the
/// channel topology.
///
/// \param[in] channel        Channel coefficient matrix.
/// \param[in] noise_variance Linear noise variance.
/// \return The TPMI information given the channel coefficients and noise variance.
pusch_tpmi_select_info get_tpmi_select_info(const srs_channel_matrix& channel, float noise_variance);

} // namespace srsran
