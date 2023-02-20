/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/downlink_processor_single_executor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "channel_processors/pdcch_processor_test_doubles.h"
#include "channel_processors/pdsch_processor_test_doubles.h"
#include "channel_processors/ssb_processor_test_doubles.h"
#include "signal_processors/nzp_csi_rs_generator_test_doubles.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/srsran_test.h"
#include "upper_phy_rg_gateway_test_doubles.h"

using namespace srsran;

static void test_works_in_order()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_spy>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_spy>();
  auto ssb_processor    = std::make_unique<ssb_processor_spy>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref  = *pdsch_processor;
  ssb_processor_spy&    ssb_ref    = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref = *csi_rs_processor;

  auto       dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  TESTASSERT(!pdcch_ref.is_process_called());
  TESTASSERT(!pdsch_ref.is_process_called());
  TESTASSERT(!ssb_ref.is_process_called());
  TESTASSERT(!csi_rs_ref.is_map_called());
  TESTASSERT(!gw.sent);

  dl_processor->process_ssb({});
  TESTASSERT(ssb_ref.is_process_called());

  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  TESTASSERT(pdcch_ref.is_process_called());

  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  TESTASSERT(pdsch_ref.is_process_called());

  dl_processor->process_nzp_csi_rs({});
  TESTASSERT(csi_rs_ref.is_map_called());

  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

static void test_finish_is_called_before_processing_pdus()
{
  upper_phy_rg_gateway_fto                gw;
  manual_task_worker_always_enqueue_tasks executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_spy>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_spy>();
  auto ssb_processor    = std::make_unique<ssb_processor_spy>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref  = *pdsch_processor;
  ssb_processor_spy&    ssb_ref    = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});

  TESTASSERT(!pdcch_ref.is_process_called());
  TESTASSERT(!pdsch_ref.is_process_called());
  TESTASSERT(!ssb_ref.is_process_called());
  TESTASSERT(!csi_rs_ref.is_map_called());
  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();
  TESTASSERT(!gw.sent);

  // Run all the queued tasks.
  executor.run_pending_tasks();

  TESTASSERT(pdcch_ref.is_process_called());
  TESTASSERT(pdsch_ref.is_process_called());
  TESTASSERT(ssb_ref.is_process_called());
  TESTASSERT(csi_rs_ref.is_map_called());

  TESTASSERT(gw.sent);

  TESTASSERT(gw.sent);
}

static void test_process_pdu_after_finish_processing_pdus_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_spy>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_spy>();
  auto ssb_processor    = std::make_unique<ssb_processor_spy>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref  = *pdsch_processor;
  ssb_processor_spy&    ssb_ref    = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->finish_processing_pdus();

  TESTASSERT(pdcch_ref.is_process_called());
  TESTASSERT(pdsch_ref.is_process_called());
  TESTASSERT(ssb_ref.is_process_called());
  TESTASSERT(gw.sent);

  // Process a PDU after finish_processing_pdus() method has been called.
  dl_processor->process_nzp_csi_rs({});
  TESTASSERT(!csi_rs_ref.is_map_called());
}

static void test_process_pdu_before_configure_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_spy>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_spy>();
  auto ssb_processor    = std::make_unique<ssb_processor_spy>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref  = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref  = *pdsch_processor;
  ssb_processor_spy&    ssb_ref    = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref = *csi_rs_processor;

  auto dl_processor = std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                                std::move(pdcch_processor),
                                                                                std::move(pdsch_processor),
                                                                                std::move(ssb_processor),
                                                                                std::move(csi_rs_processor),
                                                                                executor);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});

  TESTASSERT(!pdcch_ref.is_process_called());
  TESTASSERT(!pdsch_ref.is_process_called());
  TESTASSERT(!ssb_ref.is_process_called());
  TESTASSERT(!csi_rs_ref.is_map_called());
  TESTASSERT(!gw.sent);
}

static void test_finish_processing_before_configure_does_nothing()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_spy>(),
                                                                std::make_unique<pdsch_processor_spy>(),
                                                                std::make_unique<ssb_processor_spy>(),
                                                                std::make_unique<csi_rs_processor_spy>(),
                                                                executor);

  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(!gw.sent);
}

static void test_2consecutive_slots()
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_spy>(),
                                                                std::make_unique<pdsch_processor_spy>(),
                                                                std::make_unique<ssb_processor_spy>(),
                                                                std::make_unique<csi_rs_processor_spy>(),
                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  TESTASSERT_EQ(dl_processor->is_reserved(), false);

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);
  TESTASSERT_EQ(dl_processor->is_reserved(), true);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});
  TESTASSERT(!gw.sent);
  TESTASSERT_EQ(dl_processor->is_reserved(), true);

  dl_processor->finish_processing_pdus();

  TESTASSERT_EQ(dl_processor->is_reserved(), false);
  TESTASSERT(gw.sent);

  slot_point slot2(1, 2, 2);
  gw.clear_sent();
  resource_grid_dummy grid2;
  dl_processor->configure_resource_grid({slot2, sector}, grid2);

  dl_processor->process_ssb({});
  dl_processor->process_pdcch(pdu);
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});
  TESTASSERT(!gw.sent);

  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

static void test_finish_without_processing_pdus_sends_the_grid()
{
  upper_phy_rg_gateway_fto                gw;
  manual_task_worker_always_enqueue_tasks executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_spy>(),
                                                                std::make_unique<pdsch_processor_spy>(),
                                                                std::make_unique<ssb_processor_spy>(),
                                                                std::make_unique<csi_rs_processor_spy>(),
                                                                executor);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_spy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  // The resource grid should not have been set to zero yet.
  TESTASSERT(!grid.has_set_all_zero_method_been_called());

  // The resource grid set all zero should be enqueued.
  TESTASSERT(executor.has_pending_tasks());

  // Run resource grid zero set.
  executor.run_pending_tasks();

  // The resource grid set all zero should have been called.
  TESTASSERT(grid.has_set_all_zero_method_been_called());

  TESTASSERT(!gw.sent);

  // By finishing PDUs, the resource grid should be sent.
  dl_processor->finish_processing_pdus();

  TESTASSERT(gw.sent);
}

int main()
{
  test_works_in_order();
  test_finish_is_called_before_processing_pdus();
  test_process_pdu_after_finish_processing_pdus_does_nothing();
  test_process_pdu_before_configure_does_nothing();
  test_finish_without_processing_pdus_sends_the_grid();
  test_finish_processing_before_configure_does_nothing();
  test_2consecutive_slots();
}
