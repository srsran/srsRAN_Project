/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Clipping function.

#pragma once

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

/// \brief Clips a complex span.
///
/// Limits the amplitude of the samples to the specified clipping threshold. The clipping process is defined as
/// \f[
/// y[n] =
/// \begin{cases}
/// x[n],&  \lvert x[n] \rvert  \leq T_c \\ T_c e^{j\arg(x[n])},&  \lvert x[n] \rvert > T_c
/// \end{cases}
/// \f]
/// Where \f$ T_c \f$ is the clipping threshold.
///
/// \param [in]  x Input Span.
/// \param [in]  threshold Clipping threshold.
/// \param [out] y Output Span.
/// \return The number of clipped samples.
unsigned clip(span<const cf_t> x, float threshold, span<cf_t> y);

} // namespace srsvec
} // namespace srsgnb
