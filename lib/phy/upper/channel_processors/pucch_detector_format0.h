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

/// \file
/// PUCCH Format 0 detector declaration.

#pragma once

#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include "srsran/phy/upper/pucch_helper.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// PUCCH Format 0 detector.
class pucch_detector_format0
{
public:
  /// Creates a PUCCH Format 0 detector with its dependencies.
  pucch_detector_format0(std::unique_ptr<pseudo_random_generator>      pseudo_random_,
                         std::unique_ptr<low_papr_sequence_collection> low_papr_) :
    helper(std::move(pseudo_random_)), low_papr(std::move(low_papr_))
  {
    srsran_assert(low_papr, "");
  }

  /// Detects a PUCCH Format 0 transmission. See \ref pucch_detector for more details.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&                  grid,
                                                                 const pucch_detector::format0_configuration& config);

private:
  /// Detection threshold.
  static constexpr float detection_threshold = 4.0F;
  /// Maximum number of RE used for PUCCH Format 0. Recall that PUCCH format 0 occupies a single RB.
  static constexpr unsigned max_nof_re = NRE * MAX_PORTS * pucch_constants::format0_nof_symbols_range.stop();

  /// Temporary RE storage tensor dimensions.
  enum class dims : unsigned { re = 0, symbol = 1, rx_port = 2, nof_dims = 3 };

  /// PUCCH sequence helper.
  pucch_helper helper;
  /// Collection of low-PAPR sequences.
  std::unique_ptr<low_papr_sequence_collection> low_papr;

  /// Temporary storage of the resource elements.
  static_tensor<static_cast<unsigned>(dims::nof_dims), cf_t, max_nof_re, dims> temp_re;
  /// Temporary least square estimates for a single OFDM symbol.
  std::array<cf_t, NRE> lse;
};

} // namespace srsran
