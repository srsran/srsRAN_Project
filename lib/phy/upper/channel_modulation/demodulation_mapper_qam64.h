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

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

namespace srsgnb {

/// \brief Soft-demodulates 64-QAM modulation.
/// \param[out] llrs       Resultant log-likelihood ratios.
/// \param[in]  symbols    Input constellation symbols.
/// \param[in]  noise_vars Noise variance for each symbol in the constellation.
void demodulate_soft_QAM64(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars);

} // namespace srsgnb
