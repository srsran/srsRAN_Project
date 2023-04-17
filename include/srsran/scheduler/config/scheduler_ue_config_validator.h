/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../../../lib/scheduler/cell/cell_configuration.h"
#include "srsran/adt/expected.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {
namespace config_validators {

/// \brief Validates \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \param[in] cell_cfg Cell configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg,
                                                                   const cell_configuration&                cell_cfg);

/// \brief Validates PUCCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pucch_cfg(const sched_ue_creation_request_message& msg);

/// \brief Validates PDSCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pdsch_cfg(const sched_ue_creation_request_message& msg);

/// \brief Validates PDCCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \param[in] cell_cfg Cell configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pdcch_cfg(const sched_ue_creation_request_message& msg,
                                           const cell_configuration&                cell_cfg);

/// \brief Validates CSI-MeasConfig in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \param[in] tdd_cfg_common TDD configuration, if any.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_csi_meas_cfg(const sched_ue_creation_request_message& msg,
                                              const optional<tdd_ul_dl_config_common>& tdd_cfg_common);

} // namespace config_validators
} // namespace srsran
