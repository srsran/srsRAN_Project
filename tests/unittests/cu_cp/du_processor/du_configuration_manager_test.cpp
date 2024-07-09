/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static cu_cp_served_cell_info create_basic_served_cell_info(unsigned du_counter)
{
  cu_cp_served_cell_info cell_info;
  cell_info.nr_cgi.plmn_id = plmn_identity::test_value();
  cell_info.nr_cgi.nci     = nr_cell_identity::create({411, 22}, du_counter).value();
  cell_info.five_gs_tac    = 7;
  cell_info.nr_pci         = du_counter;
  return cell_info;
}

static du_setup_request create_basic_du_setup_request(unsigned du_counter = 0)
{
  du_setup_request req;
  req.gnb_du_id         = int_to_gnb_du_id(du_counter);
  req.gnb_du_name       = fmt::format("srsdu{}", du_counter);
  auto& cell            = req.gnb_du_served_cells_list.emplace_back();
  cell.served_cell_info = create_basic_served_cell_info(du_counter);
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

TEST_F(du_configuration_manager_test, when_two_dus_have_valid_configs_then_the_two_dus_are_added)
{
  auto setup_req1 = create_basic_du_setup_request(0);
  auto setup_req2 = create_basic_du_setup_request(1);

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req1);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req2);
  ASSERT_TRUE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 2);
}

TEST_F(du_configuration_manager_test, when_du_has_duplicate_du_id_then_setup_fails)
{
  auto setup_req1      = create_basic_du_setup_request(0);
  auto setup_req2      = create_basic_du_setup_request(1);
  setup_req2.gnb_du_id = setup_req1.gnb_du_id;

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req1);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req2);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  fmt::print("DU creation failed with error: {}\n", ret.error().cause_str);
}

TEST_F(du_configuration_manager_test, when_du_has_duplicate_nci_then_setup_fails)
{
  auto setup_req1 = create_basic_du_setup_request(0);
  auto setup_req2 = create_basic_du_setup_request(1);
  setup_req2.gnb_du_served_cells_list[0].served_cell_info.nr_cgi =
      setup_req1.gnb_du_served_cells_list[0].served_cell_info.nr_cgi;

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
