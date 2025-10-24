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

/// \file
/// \brief DM-RS PUCCH estimator implementation declaration.

#pragma once

#include "dmrs_pucch_estimator_format2.h"
#include "dmrs_pucch_estimator_formats3_4.h"
#include "srsran/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"

namespace srsran {

/// DM-RS PUCCH estimator implementation.
class dmrs_pucch_estimator_impl : public dmrs_pucch_estimator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  /// \param[in] estimator_format1_ PUCCH Format 1 DM-RS estimator.
  /// \param[in] estimator_format2_ PUCCH Format 1 DM-RS estimator.
  /// \param[in] estimator_formats3_4_ PUCCH Format 3 and Format 4 DM-RS estimator.
  dmrs_pucch_estimator_impl(std::unique_ptr<dmrs_pucch_estimator_format2>    estimator_format2_,
                            std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4_) :
    estimator_format2(std::move(estimator_format2_)), estimator_formats3_4(std::move(estimator_formats3_4_))
  {
    srsran_assert(estimator_format2, "Invalid pointer to dmrs_pucch_estimator_format2 object.");
    srsran_assert(estimator_formats3_4, "Invalid pointer to dmrs_pucch_estimator_formats3_4 object.");
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format2_configuration& config) override
  {
    estimator_format2->estimate(estimate, grid, config);
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format3_configuration& config) override
  {
    estimator_formats3_4->estimate(estimate, grid, config);
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format4_configuration& config) override
  {
    estimator_formats3_4->estimate(estimate, grid, config);
  }

private:
  /// PUCCH DM-RS estimator Format 1 component.
  std::unique_ptr<dmrs_pucch_estimator_format2> estimator_format2;
  /// PUCCH DM-RS estimator Format 3 and Format 4 component.
  std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4;
};

} // namespace srsran
