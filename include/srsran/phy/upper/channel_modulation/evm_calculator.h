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

#include "srsran/adt/complex.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {

/// Error Vector Magnitude calculator.
class evm_calculator
{
public:
  /// Default destructor.
  virtual ~evm_calculator() = default;

  /// \brief Calculates the EVM for a transmission.
  ///
  /// \param[in] soft_bits  The soft bits from the modulation demapper.
  /// \param[in] symbols    The complex-valued channel symbols, prior to the modulation demapper.
  /// \param[in] modulation The modulation scheme.
  /// \return The calculated EVM.
  /// \remark The size of \c soft_bits must be equal to the size of \c symbols times the number of bits per symbol
  /// corresponding to \c modulation.
  virtual float
  calculate(span<const log_likelihood_ratio> soft_bits, span<const cf_t> symbols, modulation_scheme modulation) = 0;
};

} // namespace srsran
