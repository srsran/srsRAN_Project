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
/// \brief Phase angle unwrapping declarations.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {
namespace srsvec {

/// \brief Computes the unwrapped arguments (phase angles) of a list of complex numbers.
///
/// \param[out] arguments     Output list of unwrapped arguments.
/// \param[in]  cplx_numbers  Input list of complex numbers.
///
/// \warning The input and output lists should have the same length.
/// \warning The memory allocated for input and output should be disjoint.
void unwrap_arguments(span<float> arguments, span<const cf_t> cplx_numbers);

} // namespace srsvec
} // namespace srsran
