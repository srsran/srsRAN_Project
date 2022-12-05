/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_cell_config_validator.h"
#include "srsgnb/scheduler/sched_consts.h"

using namespace srsgnb;
using namespace srsgnb::config_validators;

void srsgnb::config_validators::validate_sched_cell_configuration_request_message(
    const sched_cell_configuration_request_message& msg)
{
  const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    srsgnb_assert(
        pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0={}.", pdsch.k0, SCHEDULER_MAX_K0);
  }

  if (msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      srsgnb_assert(
          pusch.k2 <= SCHEDULER_MAX_K2, "k2={} value exceeds maximum supported k2={}.", pusch.k2, SCHEDULER_MAX_K2);
    }
  }

  // TODO: Validate other parameters.
}
