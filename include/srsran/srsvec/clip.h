/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
unsigned clip(span<float> y, span<const float> x, const float threshold);

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
unsigned clip_iq(span<cf_t> y, span<const cf_t> x, const float threshold);

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
unsigned clip_magnitude(span<cf_t> y, span<const cf_t> x, const float threshold);

} // namespace srsvec
} // namespace srsran
