/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/scheduler_cell_config_validator.h"
#include "srsgnb/scheduler/sched_consts.h"

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

static error_type<std::string> validate_rach_cfg_common(const sched_cell_configuration_request_message& msg)
{
  VERIFY(msg.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value(),
         "Cells without RACH-ConfigCommon are not supported");

  for (const auto& pucch_region : msg.pucch_guardbands) {
    prb_interval prach_prbs = {msg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start,
                               msg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start +
                                   1};
    VERIFY(not pucch_region.prbs.overlaps(prach_prbs),
           "Configured PRACH occasion collides with PUCCH RBs ({} intersects {})",
           pucch_region.prbs,
           prach_prbs);
  }

  return {};
}

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> srsgnb::config_validators::validate_sched_cell_configuration_request_message(
    const sched_cell_configuration_request_message& msg)
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

  HANDLE_CODE(validate_rach_cfg_common(msg));

  // TODO: Validate other parameters.
  return {};
}
