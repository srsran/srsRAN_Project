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

#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {
namespace config_validators {

/// \brief Validates \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    const auto& init_dl_bwp = cell.serv_cell_cfg.init_dl_bwp;
    if (init_dl_bwp.pdsch_cfg.has_value()) {
      const auto& dl_lst = init_dl_bwp.pdsch_cfg.value().pdsch_td_alloc_list;
      for (const auto& pdsch : dl_lst) {
        if (pdsch.k0 > SCHEDULER_MAX_K0) {
          return error_type<std::string>(fmt::format("k0={} value exceeds maximum supported k0", pdsch.k0));
        }
      }
    }

    if (cell.serv_cell_cfg.ul_config.has_value()) {
      const auto& init_ul_bwp = cell.serv_cell_cfg.ul_config.value().init_ul_bwp;
      if (init_ul_bwp.pucch_cfg.has_value()) {
        const auto& dl_to_ack_lst = init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;
        for (const auto& k1 : dl_to_ack_lst) {
          if (k1 > SCHEDULER_MAX_K1) {
            return error_type<std::string>(fmt::format("k1={} value exceeds maximum supported k1", k1));
          }
        }
      }
    }
  }

  // TODO: Validate other parameters.
  return {};
}

} // namespace config_validators
} // namespace srsgnb
