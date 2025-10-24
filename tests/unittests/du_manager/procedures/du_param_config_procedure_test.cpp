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
  du_manager_procedure_tester(std::vector<du_cell_config> cfgs = {config_helpers::make_default_du_cell_config()}) :
    cell_cfgs(cfgs), dependencies(cell_cfgs), du_mng(create_du_manager(dependencies.params))
  {
    // Generate automatic responses from F1AP and MAC.
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate.resize(cfgs.size());
    for (unsigned i = 0; i != cfgs.size(); ++i) {
      dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate[i].cgi = cell_cfgs[i].nr_cgi;
    }
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

TEST_F(du_manager_du_config_update_test, check_if_slot_time_mapping_is_available)
{
  mac_cell_time_mapper& mapper = du_mng->get_time_mapper();
  auto                  resp   = mapper.get_last_mapping();
  ASSERT_TRUE(resp.has_value());
  ASSERT_EQ(resp.value().sl_tx, slot_point(1, 1));
}

static du_param_config_request make_dummy_rrm_request()
{
  du_param_config_request req;
  req.cells.resize(1);
  req.cells[0].nr_cgi = std::nullopt;
  rrm_policy_ratio_group rrm_policy;
  rrm_policy.minimum_ratio   = 30;
  rrm_policy.maximum_ratio   = 90;
  rrm_policy.dedicated_ratio = 0;
  rrm_policy.resource_type   = rrm_policy_ratio_group::resource_type_t::prb;
  rrm_policy.policy_members_list.emplace_back(
      rrm_policy_member{.plmn_id = plmn_identity{mobile_country_code::from_string("001").value(),
                                                 mobile_network_code::from_string("01").value()},
                        .s_nssai = s_nssai_t{.sst = slice_service_type{1}}});
  req.cells[0].rrm_policy_ratio_list.emplace_back(rrm_policy);
  return req;
}

class du_manager_du_rrm_config_update_test : public du_manager_procedure_tester, public ::testing::Test
{
public:
  du_manager_du_rrm_config_update_test() :
    du_manager_procedure_tester([]() {
      std::vector<du_cell_config> cfgs;
      for (unsigned i = 0; i != NOF_CELLS; ++i) {
        cfgs.push_back(config_helpers::make_default_du_cell_config());
        cfgs.back().rrm_policy_members.emplace_back(slice_rrm_policy_config{
            .rrc_member = rrm_policy_member{.plmn_id = plmn_identity{mobile_country_code::from_string("001").value(),
                                                                     mobile_network_code::from_string("01").value()},
                                            .s_nssai = s_nssai_t{.sst = slice_service_type{1}}}});
        cfgs.back().pci        = static_cast<pci_t>(1U + i);
        cfgs.back().nr_cgi.nci = nr_cell_identity::create({411, 22}, 1 + i).value();
        cfgs.back().dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value().interleaved->shift_index = cfgs.back().pci;
      }
      return cfgs;
    }())
  {
  }

  static constexpr unsigned NOF_CELLS = 3U;
};

TEST_F(du_manager_du_rrm_config_update_test, when_rrm_policy_change_required_then_mac_is_reconfigured)
{
  // Initiate procedure.
  du_param_config_request  req  = make_dummy_rrm_request();
  du_param_config_response resp = du_mng->handle_operator_config_request(req);

  // MAC received config request.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());

  ASSERT_TRUE(resp.success);
}
