/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1u/common/f1u_connector_factory.h"
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_connector_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init logger
    f1u_logger.set_level(srslog::basic_levels::debug);
    f1u_logger.set_hex_dump_max_size(100);

    f1u_connector_creation_message msg = {};
    f1u_conn                           = create_f1u_connector(msg);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init()
  {
    logger.info("Creating F1-U connector");
    // Create test frame
  }

  std::unique_ptr<f1u_connector> f1u_conn;
  srslog::basic_logger&          logger     = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&          f1u_logger = srslog::fetch_basic_logger("F1-U", false);
};

/// Test the instantiation of a new entity
TEST_F(f1u_connector_test, create_new_connector)
{
  init();
  EXPECT_NE(f1u_conn, nullptr);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
