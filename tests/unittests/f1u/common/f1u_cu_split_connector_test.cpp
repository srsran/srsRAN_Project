/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/cu_up/split_connector/f1u_split_connector.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_cu_split_connector_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init logger
    f1u_logger_cu.set_level(srslog::basic_levels::debug);
    f1u_logger_du.set_level(srslog::basic_levels::debug);
    f1u_logger_cu.set_hex_dump_max_size(100);
    f1u_logger_du.set_hex_dump_max_size(100);

    logger.info("Creating F1-U connector");

    // create f1-u connector
    f1u_conn = std::make_unique<f1u_split_connector>();

    timers = timer_factory{timer_mng, ue_worker};

    ue_inactivity_timer = timers.create_timer();

    // prepare F1-U DU bearer config
    f1u_du_config.t_notify     = 10;
    f1u_du_config.warn_on_drop = true;

    // prepare F1-U CU-UP bearer config
    f1u_cu_up_cfg.warn_on_drop = false;
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager      timer_mng;
  manual_task_worker ue_worker{128};
  timer_factory      timers;
  unique_timer       ue_inactivity_timer;

  srs_du::f1u_config                   f1u_du_config;
  srs_cu_up::f1u_config                f1u_cu_up_cfg;
  std::unique_ptr<f1u_split_connector> f1u_conn;
  srslog::basic_logger&                logger        = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&                f1u_logger_cu = srslog::fetch_basic_logger("CU-F1-U", false);
  srslog::basic_logger&                f1u_logger_du = srslog::fetch_basic_logger("DU-F1-U", false);
};
} // namespace

/// Test the instantiation of a new entity
TEST_F(f1u_cu_split_connector_test, create_new_connector)
{
  EXPECT_NE(f1u_conn, nullptr);
  EXPECT_NE(f1u_conn->get_f1u_cu_up_gateway(), nullptr);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
