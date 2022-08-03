/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi_adaptor/phy/fapi_to_phy_translator.h"
#include "../../fapi/validators/helpers.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "../../phy/upper/downlink_processor_test_doubles.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;
using namespace unittest;

static constexpr subcarrier_spacing scs_common = subcarrier_spacing::kHz15;

namespace {
static resource_grid_spy grid;

class resource_grid_pool_dummy : public resource_grid_pool
{
  resource_grid& grid;

public:
  resource_grid_pool_dummy(resource_grid& grid) : grid(grid) {}

  resource_grid& get_resource_grid(const srsgnb::resource_grid_context& context) override { return grid; }
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

  downlink_processor& get_processor(const slot_point& slot, unsigned sector_id) override
  {
    return dl_processor[slot.slot_index()];
  }

  const downlink_processor_spy& processor(const slot_point& slot) const { return dl_processor[slot.slot_index()]; }
};

} // namespace

static void test_downlink_processor_is_configured_on_new_slot()
{
  resource_grid_spy             grid;
  downlink_processor_pool_dummy dl_processor_pool;
  resource_grid_pool_dummy      rg_pool(grid);
  unsigned                      sector_id = 0;

  fapi_to_phy_translator translator(sector_id, dl_processor_pool, rg_pool, scs_common);
  slot_point             slot(1, 1, 0);

  TESTASSERT(!dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  TESTASSERT(!grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  // Assert that the downlink processor is configured.
  TESTASSERT(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());

  // Assert that the resource grid has been set to zero.
  TESTASSERT(grid.has_set_all_zero_method_been_called());
}

static void test_current_grid_is_send_on_new_slot()
{
  resource_grid_spy             grid;
  downlink_processor_pool_dummy dl_processor_pool;
  resource_grid_pool_dummy      rg_pool(grid);
  unsigned                      sector_id = 0;

  fapi_to_phy_translator translator(sector_id, dl_processor_pool, rg_pool, scs_common);
  slot_point             slot(1, 1, 0);

  TESTASSERT(!dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  TESTASSERT(!grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  // Assert that the downlink processor is configured.
  TESTASSERT(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  // Assert that the resource grid has been set to zero.
  TESTASSERT(grid.has_set_all_zero_method_been_called());

  slot_point slot2(1, 1, 1);
  translator.handle_new_slot(slot2);

  // Assert that the finish processing PDUs method of the previous slot downlink_processor has been called.
  TESTASSERT(dl_processor_pool.processor(slot).has_finish_processing_pdus_method_been_called());
  TESTASSERT(dl_processor_pool.processor(slot2).has_configure_resource_grid_method_been_called());
  TESTASSERT(!dl_processor_pool.processor(slot2).has_finish_processing_pdus_method_been_called());
}

static void test_dl_ssb_pdu_is_processed()
{
  resource_grid_spy             grid;
  downlink_processor_pool_dummy dl_processor_pool;
  resource_grid_pool_dummy      rg_pool(grid);
  unsigned                      sector_id = 0;

  fapi_to_phy_translator translator(sector_id, dl_processor_pool, rg_pool, scs_common);
  slot_point             slot(1, 1, 0);

  TESTASSERT(!dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  TESTASSERT(!grid.has_set_all_zero_method_been_called());

  translator.handle_new_slot(slot);

  // Assert that the downlink processor is configured.
  TESTASSERT(dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  TESTASSERT(!dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
  // Assert that the resource grid has been set to zero.
  TESTASSERT(grid.has_set_all_zero_method_been_called());

  // Process SSB PDU.
  const dl_tti_request_message& msg = build_valid_dl_tti_request();
  translator.dl_tti_request(msg);

  // Assert that the SSB PDU has been processed.
  TESTASSERT(dl_processor_pool.processor(slot).has_process_ssb_method_been_called());

  slot_point slot2(1, 1, 1);
  translator.handle_new_slot(slot2);

  // Assert that the finish processing PDUs method of the previous slot downlink_processor has been called.
  TESTASSERT(dl_processor_pool.processor(slot).has_finish_processing_pdus_method_been_called());
  TESTASSERT(dl_processor_pool.processor(slot2).has_configure_resource_grid_method_been_called());
  TESTASSERT(!dl_processor_pool.processor(slot2).has_finish_processing_pdus_method_been_called());
}

static void test_calling_dl_tti_request_without_handling_slot_does_nothing()
{
  resource_grid_spy             grid;
  downlink_processor_pool_dummy dl_processor_pool;
  resource_grid_pool_dummy      rg_pool(grid);
  unsigned                      sector_id = 0;

  fapi_to_phy_translator translator(sector_id, dl_processor_pool, rg_pool, scs_common);
  slot_point             slot(1, 1, 0);

  TESTASSERT(!dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
  TESTASSERT(!dl_processor_pool.processor(slot).has_configure_resource_grid_method_been_called());
  TESTASSERT(!grid.has_set_all_zero_method_been_called());

  // Process SSB PDU.
  const dl_tti_request_message& msg = build_valid_dl_tti_request();
  translator.dl_tti_request(msg);
  TESTASSERT(!dl_processor_pool.processor(slot).has_process_ssb_method_been_called());
}

int main()
{
  test_calling_dl_tti_request_without_handling_slot_does_nothing();
  test_downlink_processor_is_configured_on_new_slot();
  test_current_grid_is_send_on_new_slot();
  test_dl_ssb_pdu_is_processed();
  fmt::print("FAPI to PHY translator -> OK \n");
}
