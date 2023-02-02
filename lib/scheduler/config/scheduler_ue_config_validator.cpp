/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/scheduler_ue_config_validator.h"

using namespace srsgnb;
using namespace config_validators;

#define VERIFY(cond, ...)                                                                                              \
  if (not(cond)) {                                                                                                     \
    return error_type<std::string>(fmt::format(__VA_ARGS__));                                                          \
  }
#define HANDLE_CODE(cond)                                                                                              \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (ret.is_error()) {                                                                                              \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

error_type<std::string> srsgnb::config_validators::validate_pucch_cfg(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    if (cell.serv_cell_cfg.ul_config.has_value()) {
      const auto& init_ul_bwp = cell.serv_cell_cfg.ul_config.value().init_ul_bwp;
      if (init_ul_bwp.pucch_cfg.has_value()) {
        const auto& dl_to_ack_lst = init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;
        for (const auto& k1 : dl_to_ack_lst) {
          VERIFY(k1 <= SCHEDULER_MAX_K1, "k1={} value exceeds maximum supported k1", k1);
        }
      }
    }
  }
  // TODO: Validate other parameters.
  return {};
}

error_type<std::string> srsgnb::config_validators::validate_pdsch_cfg(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    const auto& init_dl_bwp = cell.serv_cell_cfg.init_dl_bwp;
    if (init_dl_bwp.pdsch_cfg.has_value()) {
      const auto& dl_lst = init_dl_bwp.pdsch_cfg.value().pdsch_td_alloc_list;
      for (const auto& pdsch : dl_lst) {
        VERIFY(pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0", pdsch.k0);
      }
    }
  }
  // TODO: Validate other parameters.
  return {};
}

error_type<std::string> srsgnb::config_validators::validate_csi_meas_cfg(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    if (cell.serv_cell_cfg.pdsch_serv_cell_cfg.has_value()) {
    }
  }
  // TODO: Validate other parameters.
  return {};
}

error_type<std::string>
srsgnb::config_validators::validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg)
{
  HANDLE_CODE(validate_pdsch_cfg(msg));

  HANDLE_CODE(validate_pucch_cfg(msg));

  HANDLE_CODE(validate_csi_meas_cfg(msg));

  // TODO: Validate other parameters.
  return {};
}
