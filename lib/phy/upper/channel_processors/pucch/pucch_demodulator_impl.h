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
/// \brief PUCCH demodulator implementation declaration.

#pragma once

#include "pucch_demodulator_format2.h"
#include "pucch_demodulator_format3.h"
#include "pucch_demodulator_format4.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"

namespace srsran {

/// PUCCH demodulator implementation.
class pucch_demodulator_impl : public pucch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  /// \param[in] demodulator_format2_ PUCCH Format 2 demodulator.
  /// \param[in] demodulator_format3_ PUCCH Format 3 demodulator.
  pucch_demodulator_impl(std::unique_ptr<pucch_demodulator_format2> demodulator_format2_,
                         std::unique_ptr<pucch_demodulator_format3> demodulator_format3_,
                         std::unique_ptr<pucch_demodulator_format4> demodulator_format4_) :
    demodulator_format2(std::move(demodulator_format2_)),
    demodulator_format3(std::move(demodulator_format3_)),
    demodulator_format4(std::move(demodulator_format4_))
  {
    srsran_assert(demodulator_format2, "Invalid pointer to pucch_demodulator_format2 object.");
    srsran_assert(demodulator_format3, "Invalid pointer to pucch_demodulator_format3 object.");
    srsran_assert(demodulator_format4, "Invalid pointer to pucch_demodulator_format4 object.");
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format2_configuration& config) override
  {
    demodulator_format2->demodulate(llr, grid, estimates, config);
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format3_configuration& config) override
  {
    demodulator_format3->demodulate(llr, grid, estimates, config);
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format4_configuration& config) override
  {
    demodulator_format4->demodulate(llr, grid, estimates, config);
  }

private:
  /// PUCCH demodulator Format 2 component.
  std::unique_ptr<pucch_demodulator_format2> demodulator_format2;
  /// PUCCH demodulator Format 3 component.
  std::unique_ptr<pucch_demodulator_format3> demodulator_format3;
  /// PUCCH demodulator Format 4 component.
  std::unique_ptr<pucch_demodulator_format4> demodulator_format4;
};

} // namespace srsran
