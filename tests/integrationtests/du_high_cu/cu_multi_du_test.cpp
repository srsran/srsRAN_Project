/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_cu_test_simulator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

class cu_multi_du_test : public du_high_cu_test_simulator, public ::testing::Test
{
public:
  cu_multi_du_test() :
    du_high_cu_test_simulator(du_high_cu_cp_test_simulator_config{
        {{{config_helpers::make_default_du_cell_config()}}, {{config_helpers::make_default_du_cell_config()}}}})
  {
  }
};

TEST_F(cu_multi_du_test, setup_multiple_dus)
{
  ASSERT_EQ(this->cu_cp_inst->get_connected_dus().get_nof_dus(), 2);
}