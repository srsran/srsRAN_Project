/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/ran/modulation_scheme.h"

namespace srsgnb {

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

} // namespace srsgnb
