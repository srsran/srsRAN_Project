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

#include "du_low_config.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"

namespace srsran {

/// O-RAN DU low unit HAL dependencies.
struct o_du_low_hal_dependencies {
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = nullptr;
  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory = nullptr;
};

/// \brief Initializes the HAL depencies of the DU low unit.
/// \param[out] hal_config Struct defining the DU low HAL configuration
/// \return  Struct containing the DU low unit dependencies.
o_du_low_hal_dependencies make_du_low_hal_dependencies(const std::optional<du_low_unit_hal_config>& hal_config);

} // namespace srsran
