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

#include "../du_manager_test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_high/du_manager/du_manager_factory.h"
#include "srsran/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class du_manager_procedure_tester
{
public:
  du_manager_procedure_tester() :
    cell_cfgs({config_helpers::make_default_du_cell_config()}),
    dependencies(cell_cfgs),
    du_mng(create_du_manager(dependencies.params))
  {
    // Generate automatic responses from F1AP and MAC.
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate.resize(1);
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate[0].cgi = cell_cfgs[0].nr_cgi;
    dependencies.f1ap.wait_f1_setup.ready_ev.set();
    dependencies.f1ap.wait_f1_removal.ready_ev.set();
    dependencies.mac.mac_cell.wait_start.ready_ev.set();
    dependencies.mac.mac_cell.wait_stop.ready_ev.set();

    // Start DU manager.
    du_mng->start();
  }
  ~du_manager_procedure_tester()
  {
    std::atomic<bool> done{false};
    worker.push_task_blocking([this, &done]() {
      du_mng->stop();
      done = true;
    });
    while (not done) {
      dependencies.worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    worker.wait_pending_tasks();
  }

  task_worker                           worker{"worker", 16};
  std::vector<du_cell_config>           cell_cfgs;
  du_manager_test_bench                 dependencies;
  std::unique_ptr<du_manager_interface> du_mng;
};

static du_param_config_request make_dummy_request(span<const du_cell_config> cell_cfgs)
{
  du_param_config_request req;
  req.cells.resize(1);
  req.cells[0].nr_cgi      = cell_cfgs[0].nr_cgi;
  req.cells[0].ssb_pwr_mod = cell_cfgs[0].ssb_cfg.ssb_block_power + 1;
  return req;
}

class du_manager_du_config_update_test : public du_manager_procedure_tester, public ::testing::Test
{};

TEST_F(du_manager_du_config_update_test, when_sib1_change_required_then_mac_is_reconfigured_and_f1ap_initiates_du_cfg)
{
  // Initiate procedure.
  du_param_config_request  req  = make_dummy_request(cell_cfgs);
  du_param_config_response resp = du_mng->handle_operator_config_request(req);

  // MAC received config request.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());

  // F1AP received the DU config request.
  ASSERT_TRUE(dependencies.f1ap.last_du_cfg_req);

  ASSERT_TRUE(resp.success);
}
