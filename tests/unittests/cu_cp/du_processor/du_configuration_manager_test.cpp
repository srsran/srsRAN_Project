/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/cu_cp/du_processor/du_configuration_manager.h"
#include "srsran/rrc/rrc_config.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

static rrc_cfg_t create_basic_rrc_config()
{
  rrc_cfg_t cfg{};
  cfg.gnb_id = {411, 22};
  return cfg;
}

static cu_cp_served_cell_info create_basic_served_cell_info()
{
  cu_cp_served_cell_info cell_info;
  cell_info.nr_cgi.plmn_id = plmn_identity::test_value();
  cell_info.nr_cgi.nci     = nr_cell_identity::create({411, 22}, 0x1).value();
  cell_info.five_gs_tac    = 7;
  cell_info.nr_pci         = 1;
  return cell_info;
}

static du_setup_request create_basic_du_setup_request()
{
  du_setup_request req;
  req.gnb_du_id         = gnb_du_id_t::min;
  req.gnb_du_name       = "srsdu";
  auto& cell            = req.gnb_du_served_cells_list.emplace_back();
  cell.served_cell_info = create_basic_served_cell_info();
  cell.gnb_du_sys_info.emplace();
  cell.gnb_du_sys_info->mib_msg  = byte_buffer::create({0x0, 0x1, 0x2}).value();
  cell.gnb_du_sys_info->sib1_msg = byte_buffer::create({0x3, 0x4, 0x5}).value();
  return req;
}

class du_configuration_manager_test : public ::testing::Test
{
public:
  du_configuration_manager_test() : du_cfg_mng(rrc_cfg) {}

  rrc_cfg_t                rrc_cfg = create_basic_rrc_config();
  du_configuration_manager du_cfg_mng;
};

TEST_F(du_configuration_manager_test, when_instance_created_then_it_has_no_dus)
{
  ASSERT_EQ(du_cfg_mng.nof_dus(), 0);
}

TEST_F(du_configuration_manager_test, when_du_config_handler_is_created_then_it_has_no_context)
{
  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  ASSERT_NE(du_cfg_updater, nullptr);
  ASSERT_FALSE(du_cfg_updater->has_context());
}

TEST_F(du_configuration_manager_test, when_du_is_setup_successfully_then_context_is_updated)
{
  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto setup_req      = create_basic_du_setup_request();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
  ASSERT_TRUE(ret.has_value()) << "DU setup failed: " << ret.error().cause_str;
  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
}

TEST_F(du_configuration_manager_test, when_du_cfg_handler_goes_out_of_scope_then_du_config_is_removed)
{
  {
    auto du_cfg_updater = du_cfg_mng.create_du_handler();
    auto setup_req      = create_basic_du_setup_request();
    auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
    ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  }
  ASSERT_EQ(du_cfg_mng.nof_dus(), 0);
}

TEST_F(du_configuration_manager_test, when_du_has_duplicate_du_id_then_setup_fails)
{
  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto setup_req      = create_basic_du_setup_request();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req);
  ASSERT_FALSE(ret.has_value());
  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  fmt::print("DU creation failed with error: {}\n", ret.error().cause_str);
}
