/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_ue_config_validator.h"
#include "srsgnb/scheduler/sched_consts.h"

using namespace srsgnb;
using namespace srsgnb::config_validators;

void srsgnb::config_validators::validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg)
{
  for (const auto& cell : msg.cells) {
    if (cell.serv_cell_cfg.has_value()) {
      const auto& init_dl_bwp = cell.serv_cell_cfg.value().init_dl_bwp;
      if (init_dl_bwp.pdsch_cfg.has_value()) {
        const auto& dl_lst = init_dl_bwp.pdsch_cfg.value().pdsch_td_alloc_list;
        for (const auto& pdsch : dl_lst) {
          srsgnb_assert(
              pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0={}.", pdsch.k0, SCHEDULER_MAX_K0);
        }
      }

      if (cell.serv_cell_cfg.value().ul_config.has_value()) {
        const auto& init_ul_bwp = cell.serv_cell_cfg.value().ul_config.value().init_ul_bwp;
        if (init_ul_bwp.pucch_cfg.has_value()) {
          const auto& dl_to_ack_lst = init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;
          for (const auto& k1 : dl_to_ack_lst) {
            srsgnb_assert(k1 <= SCHEDULER_MAX_K1, "k1={} value exceeds maximum supported k1={}.", k1, SCHEDULER_MAX_K1);
          }
        }
      }
    }
  }

  // TODO: Validate other parameters.
}
