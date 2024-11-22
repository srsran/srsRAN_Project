/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
/// \param[in]  nof_cells  Number of cells to be handled by the HAL.
/// \return  Struct containing the DU low unit dependencies.
o_du_low_hal_dependencies make_du_low_hal_dependencies(const std::optional<du_low_unit_hal_config>& hal_config,
                                                       unsigned                                     nof_cells);

} // namespace srsran
