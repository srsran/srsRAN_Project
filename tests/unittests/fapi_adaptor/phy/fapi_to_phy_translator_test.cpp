/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../../lib/fapi_adaptor/phy/fapi_to_phy_translator.h"
#include "../../fapi/validators/helpers.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "../../phy/upper/downlink_processor_test_doubles.h"
#include "../../phy/upper/uplink_request_processor_test_doubles.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/uplink_slot_pdu_repository.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittest;

static constexpr subcarrier_spacing scs_common = subcarrier_spacing::kHz15;

namespace {

class downlink_pdu_validator_dummy : public downlink_pdu_validator
{
public:
  bool is_valid(const ssb_processor::pdu_t& pdu) const override { return true; }
  bool is_valid(const pdcch_processor::pdu_t& pdu) const override { return true; }
  bool is_valid(const pdsch_processor::pdu_t& pdu) const override { return true; }
  bool is_valid(const nzp_csi_rs_generator::config_t& config) const override { return true; }
};

class uplink_pdu_validator_dummy : public uplink_pdu_validator
{
public:
  bool is_valid(const prach_detector::configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format0_configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format1_configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format2_configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format3_configuration& config) const override { return true; }
  bool is_valid(const pucch_processor::format4_configuration& config) const override { return true; }
  bool is_valid(const pusch_processor::pdu_t& pdu) const override { return true; }
};

class resource_grid_pool_dummy : public resource_grid_pool
{
  resource_grid& grid;

public:
  explicit resource_grid_pool_dummy(resource_grid& grid_) : grid(grid_) {}

  resource_grid& get_resource_grid(const srsran::resource_grid_context& context) override { return grid; }
};

class downlink_processor_pool_dummy : public downlink_processor_pool
{
  static_vector<downlink_processor_spy, 2> dl_processor;

public:
  downlink_processor_pool_dummy()
  {
    dl_processor.emplace_back(0);
    dl_processor.emplace_back(1);
  }

  downlink_processor& get_processor(slot_point slot, unsigned sector_id) override
  {
    return dl_processor[slot.slot_index()];
  }

  const downlink_processor_spy& processor(slot_point slot) const { return dl_processor[slot.slot_index()]; }
};

} // namespace

class fapi_to_phy_translator_fixture : public ::testing::Test
{
protected:
  resource_grid_spy              grid;
  downlink_processor_pool_dummy  dl_processor_pool;
  resource_grid_pool_dummy       rg_pool;
  uplink_request_processor_dummy ul_request_processor;
  uplink_slot_pdu_repository     pdu_repo;
  unsigned                       sector_id = 0;
  fapi::prach_config             prach_cfg;
  fapi::carrier_config           carrier_cfg = {0, 0, {}, {11, 51, 106, 0, 0}, 0, 0, 0, {}, {}, 0, 0, 0, 0};
  downlink_pdu_validator_dummy   dl_pdu_validator;
  uplink_pdu_validator_dummy     ul_pdu_validator;
  fapi_to_phy_translator_config  config = {sector_id,
                                           subcarrier_spacing::kHz15,
                                           &dl_processor_pool,
                                           &rg_pool,
                                           &ul_request_processor,
                                           &rg_pool,
                                           &pdu_repo,
                                           &dl_pdu_validator,
                                           &ul_pdu_validator,
                                           scs_common,
                                           &prach_cfg,
                                           &carrier_cfg,
                                           std::move(std::get<1>(generate_precoding_matrix_tables(1)))};
  fapi_to_phy_translator         translator;

public:
  fapi_to_phy_translator_fixture() :
    grid(0, 0, 0), rg_pool(grid), pdu_repo(2), translator(std::move(config), srslog::fetch_basic_logger("FAPI"))
  {
  }
};

TEST_F(fapi_to_phy_translator_fixture, downlink_processor_is_not_configured_on_new_slot)
{
  slot_point slot(1, 1, 0);

  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  // Assert that the downlink processor is configured.
  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());
}

TEST_F(fapi_to_phy_translator_fixture, downlink_processor_is_configured_on_new_dl_tti_request)
{
  slot_point slot(1, 1, 0);

  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  fapi::dl_tti_request_message msg;
  msg.sfn  = slot.sfn();
  msg.slot = slot.slot_index();

  translator.dl_tti_request(msg);

  // Assert that the downlink processor is configured.
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());

  // Assert that the resource grid has not been set to zero.
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());
}

TEST_F(fapi_to_phy_translator_fixture, current_grid_is_sent_on_new_slot)
{
  slot_point slot(1, 1, 0);

  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  fapi::dl_tti_request_message msg;
  msg.sfn  = slot.sfn();
  msg.slot = slot.slot_index();

  translator.dl_tti_request(msg);

  // Assert that the downlink processor is configured.
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  // Assert that the resource grid has NOT been set to zero.
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  slot_point slot2(1, 1, 1);
  translator.handle_new_slot(slot2);

  // Assert that the finish processing PDUs method of the previous slot downlink_processor has been called.
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_finish_processing_pdus_method_been_called());
}

TEST_F(fapi_to_phy_translator_fixture, dl_ssb_pdu_is_processed)
{
  const fapi::dl_tti_request_message& msg = build_valid_dl_tti_request();
  slot_point                          slot(subcarrier_spacing::kHz15, msg.sfn, msg.slot);

  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);
  // Process SSB PDU.
  translator.dl_tti_request(msg);

  // Assert that the downlink processor is configured.
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
  // Assert that the resource grid has NOT been set to zero.
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  slot_point slot2 = slot + 1;
  translator.handle_new_slot(slot2);

  // Assert that the finish processing PDUs method of the previous slot downlink_processor has been called.
  ASSERT_TRUE(dl_processor_pool.processor(slot).has_finish_processing_pdus_method_been_called());
}

TEST_F(fapi_to_phy_translator_fixture, calling_dl_tti_request_without_handling_slot_does_nothing)
{
  slot_point slot(1, 1, 0);

  ASSERT_FALSE(dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
  ASSERT_FALSE(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  // Process SSB PDU.
  const fapi::dl_tti_request_message& msg = build_valid_dl_tti_request();
  translator.dl_tti_request(msg);
  ASSERT_FALSE(dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
}
