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
/// \brief PUCCH detector declaration.

#pragma once

#include "pucch_detector_format0.h"
#include "pucch_detector_format1.h"
#include "srsran/phy/upper/channel_processors/pucch_detector.h"

namespace srsran {

/// PUCCH detector implementation for Formats 0 and 1.
class pucch_detector_impl : public pucch_detector
{
public:
  /// \brief Constructor: provides access to the actual Format 0 and Format 1 detectors.
  /// \param[in] detector_format0_ PUCCH Format 0 detector.
  /// \param[in] detector_format1_ PUCCH Format 1 detector.
  pucch_detector_impl(std::unique_ptr<pucch_detector_format0> detector_format0_,
                      std::unique_ptr<pucch_detector_format1> detector_format1_) :
    detector_format0(std::move(detector_format0_)), detector_format1(std::move(detector_format1_))
  {
    srsran_assert(detector_format0, "PUCCH Format 0 detector.");
    srsran_assert(detector_format1, "PUCCH Format 1 detector.");
  }

  // See interface for documentation.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                 const format0_configuration& config) override
  {
    return detector_format0->detect(grid, config);
  }

  // See interface for documentation.
  pucch_detection_result detect(const resource_grid_reader&  grid,
                                const channel_estimate&      estimates,
                                const format1_configuration& config) override
  {
    return detector_format1->detect(grid, estimates, config);
  }

private:
  /// PUCCH Format 0 detector.
  std::unique_ptr<pucch_detector_format0> detector_format0;
  /// PUCCH Format 1 detector.
  std::unique_ptr<pucch_detector_format1> detector_format1;
};

} // namespace srsran
