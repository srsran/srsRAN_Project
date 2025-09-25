/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
