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
/// \brief Clipping functions.

#pragma once

#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// \brief Clips a span of floats.
///
/// Limits the amplitude of the samples to the specified clipping threshold. The clipping process is defined as
/// \f[
/// y[n] =
/// \begin{cases}
/// x[n],&  \lvert x[n] \rvert  \leq T_c \\ T_c,& x[n] > T_c \\ -T_c,& x[n] < -T_c
/// \end{cases}
/// \f]
/// Where \f$ T_c \f$ is the clipping threshold.
///
/// \param [out] y Output Span.
/// \param [in]  x Input Span.
/// \param [in]  threshold Clipping threshold.
/// \return The number of clipped samples.
unsigned clip(span<float> y, span<const float> x, float threshold);

/// \brief Clips the real and imaginary components of a complex span.
///
/// Limits the amplitude of the real and imaginary components of the input samples to the specified clipping threshold.
/// The clipping process is defined as
/// \f[
/// \Re(y[n]) =
/// \begin{cases}
/// \Re \{x[n]\},&  \lvert \Re\{x[n]\} \rvert  \leq T_c \\ T_c,& \Re\{x[n]\} > T_c \\ -T_c,& \Re \{x[n]\} < -T_c
/// \end{cases}
/// \Im(y[n]) =
/// \begin{cases}
/// \Im \{x[n]\},&  \lvert \Im\{x[n]\} \rvert  \leq T_c \\ T_c,& \Im\{x[n]\} > T_c \\ -T_c,& \Im \{x[n]\} < -T_c
/// \end{cases}
/// \f]
///
/// Where \f$ T_c \f$ is the clipping threshold.
///
/// \param [out] y Output Span.
/// \param [in]  x Input Span.
/// \param [in]  threshold Clipping threshold.
/// \return The number of clipped samples.
unsigned clip_iq(span<cf_t> y, span<const cf_t> x, float threshold);

/// \brief Clips the magnitude of a complex span.
///
/// Limits the magnitude of the samples to the specified clipping threshold. The clipping process is defined as
/// \f[
/// y[n] =
/// \begin{cases}
/// x[n],&  \lvert x[n] \rvert  \leq T_c \\ T_c e^{j\arg(x[n])},&  \lvert x[n] \rvert > T_c
/// \end{cases}
/// \f]
/// Where \f$ T_c \f$ is the clipping threshold.
///
/// \param [out] y Output Span.
/// \param [in]  x Input Span.
/// \param [in]  threshold Clipping threshold.
/// \return The number of clipped samples.
unsigned clip_magnitude(span<cf_t> y, span<const cf_t> x, float threshold);

} // namespace srsvec
} // namespace srsran
