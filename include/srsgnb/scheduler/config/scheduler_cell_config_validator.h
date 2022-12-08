/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {
namespace config_validators {

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string>
validate_sched_cell_configuration_request_message(const sched_cell_configuration_request_message& msg)
{
  const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    if (pdsch.k0 > SCHEDULER_MAX_K0) {
      return error_type<std::string>(fmt::format("k0={} value exceeds maximum supported k0", pdsch.k0));
    }
  }

  if (msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > SCHEDULER_MAX_K2) {
        return error_type<std::string>(fmt::format("k2={} value exceeds maximum supported k2", pusch.k2));
      }
    }
  }

  // TODO: Validate other parameters.
  return {};
}

} // namespace config_validators
} // namespace srsgnb
