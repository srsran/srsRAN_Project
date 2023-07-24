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

#include "serving_cell_config.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/support/config/validator_result.h"

namespace srsran {
namespace config_validators {

/// \brief Validates PDCCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] ue_cell_cfg UE serving cell configuration to be validated.
/// \param[in] dl_cfg_common Cell DL common configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
validator_result validate_pdcch_cfg(const serving_cell_config& ue_cell_cfg, const dl_config_common& dl_cfg_common);

/// \brief Validates PDSCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] ue_cell_cfg UE serving cell configuration to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
validator_result validate_pdsch_cfg(const serving_cell_config& ue_cell_cfg);

/// \brief Validates PUCCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] ue_cell_cfg UE serving cell configuration to be validated.
/// \param[in] nof_dl_antennas Number of antennas used for DL tx.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pucch_cfg(const serving_cell_config& ue_cell_cfg, unsigned nof_dl_antennas);

/// \brief Validates CSI-MeasConfig in \c serving_cell_config passed to a UE.
/// \param[in] ue_cell_cfg UE serving cell configuration to be validated.
/// \param[in] tdd_cfg_common TDD configuration, if any.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
validator_result validate_csi_meas_cfg(const serving_cell_config&               ue_cell_cfg,
                                       const optional<tdd_ul_dl_config_common>& tdd_cfg_common);

} // namespace config_validators
} // namespace srsran