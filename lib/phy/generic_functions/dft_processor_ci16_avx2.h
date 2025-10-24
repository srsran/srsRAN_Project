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

#pragma once

#include "srsran/phy/generic_functions/dft_processor_ci16.h"

namespace srsran {

/// \brief Creates a DFT processor for 16-bit complex integer samples.
///
/// The DFT processor is templated to reduce interfaces overhead and this method isolates the factory source file from
/// including processor specific intrinsics headers.
///
/// \param dft_size DFT size
/// \param direction DFT direction.
/// \return A DFT processor if combination of DFT size and direction is implemented, otherwise nullptr.
std::unique_ptr<dft_processor_ci16> create_dft_processor_ci16_avx2(unsigned                      dft_size,
                                                                   dft_processor_ci16::direction direction);

} // namespace srsran
