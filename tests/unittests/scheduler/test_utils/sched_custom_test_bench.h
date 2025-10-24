/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

namespace srsran {

class dummy_harq_timeout_notifier : public harq_timeout_notifier
{
public:
  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override {}
};

class sched_basic_custom_test_bench
{
public:
  sched_basic_custom_test_bench(const scheduler_expert_config&                  sched_exp_cfg,
                                const sched_cell_configuration_request_message& sched_cell_cfg_req) :
    expert_cfg{sched_exp_cfg},
    cell_cfg{[this, sched_cell_cfg_req]() -> const cell_configuration& {
      cfg_pool.add_cell(sched_cell_cfg_req);
      cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(expert_cfg, sched_cell_cfg_req));
      return *cell_cfg_list[to_du_cell_index(0)];
    }()},
    cell_harqs{MAX_NOF_DU_UES, MAX_NOF_HARQS, std::make_unique<dummy_harq_timeout_notifier>()},
    cell_ues(ues.add_cell(to_du_cell_index(0))),
    current_sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    slot_indication(current_sl_tx);
    mac_logger.set_level(srslog::basic_levels::debug);
  }

  // Class members.
  scheduler_expert_config        expert_cfg;
  cell_common_configuration_list cell_cfg_list{};
  du_cell_group_config_pool      cfg_pool;
  const cell_configuration&      cell_cfg;
  cell_harq_manager              cell_harqs;
  ue_repository                  ues;
  ue_cell_repository&            cell_ues;
  std::vector<ue_configuration>  ue_ded_cfgs;
  cell_resource_allocator        res_grid{cell_cfg};
  slot_point                     current_sl_tx;

  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  // Class methods.
  void add_ue(sched_ue_creation_request_message ue_req)
  {
    ue_ded_cfgs.emplace_back(ue_req.ue_index, ue_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_req));
    ues.add_ue(std::make_unique<ue>(ue_creation_command{ue_ded_cfgs.back(), ue_req.starts_in_fallback, cell_harqs}));
  }

  void slot_indication(slot_point slot_tx)
  {
    mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
    res_grid.slot_indication(slot_tx);
  }
};

} // namespace srsran
