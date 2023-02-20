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

#include "srsgnb/adt/expected.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsran {
namespace config_validators {

/// \brief Validates \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg);

/// \brief Validates PUCCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pucch_cfg(const sched_ue_creation_request_message& msg);

/// \brief Validates PDSCH Config in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_pdsch_cfg(const sched_ue_creation_request_message& msg);

/// \brief Validates CSI-MeasConfig in \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_csi_meas_cfg(const sched_ue_creation_request_message& msg);

} // namespace config_validators
} // namespace srsran
