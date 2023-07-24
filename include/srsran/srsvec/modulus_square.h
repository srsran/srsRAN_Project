/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/srsvec/types.h"

namespace srsran {

namespace srsvec {

/// \brief Calculates the element-wise modulus square of a sequence of complex values.
/// \param[out] result      Output non-negative real values.
/// \param[in]  input       Input complex values.
/// \remark An assertion is triggered if \c input and \c result do not have the same number of elements.
void modulus_square(span<float> result, span<const cf_t> input);

} // namespace srsvec
} // namespace srsran
