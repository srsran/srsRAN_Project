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
#include "srsran/srslog/srslog.h"

#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#endif // DPDK_FOUND

namespace srsran {

/// ORAN DU low unit HAL dependencies.
struct o_du_low_hal_dependencies {
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = nullptr;
  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory = nullptr;
};

#ifdef DPDK_FOUND
/// \brief Creates and initializes a bbdev-based hardware accelerator.
/// \param[in] bbdev_app_cfg Struct containing the DU low HAL configuration.
/// \param[in] logger        SRS logger.
/// \param[in] nof_hwacc_dus Number of DU-low instances to be managed by the hardware accelerator.
/// \return A pointer to the bbdev-based accelerator interface on success, nullptr otherwise.
static std::shared_ptr<dpdk::bbdev_acc>
init_bbdev_hwacc(const bbdev_appconfig& bbdev_app_cfg, srslog::basic_logger& logger, unsigned nof_hwacc_dus);
#endif // DPDK_FOUND

/// \brief Initializes the HAL depencies of the DU low unit.
/// \param[out] hal_dependencies Struct containing the DU low unit dependencies.
/// \param[in]  du_low_unit_cfg  Struct defining the DU low configuration.
/// \param[in]  nof_cells        Number of cells to be handled by the HAL.
o_du_low_hal_dependencies make_du_low_hal_dependencies(const du_low_unit_config& du_low_unit_cfg, unsigned nof_cells);

} // namespace srsran
