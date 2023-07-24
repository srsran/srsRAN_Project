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

#include "../../../lib/phy/upper/downlink_processor_single_executor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "channel_processors/pdcch_processor_test_doubles.h"
#include "channel_processors/pdsch_processor_test_doubles.h"
#include "channel_processors/ssb_processor_test_doubles.h"
#include "signal_processors/nzp_csi_rs_generator_test_doubles.h"
#include "upper_phy_rg_gateway_test_doubles.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "gtest/gtest.h"

using namespace srsran;

TEST(downlinkProcessorTest, worksInOrder)
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

  ASSERT_FALSE(pdcch_ref.is_process_called());
  ASSERT_FALSE(pdsch_ref.is_process_called());
  ASSERT_FALSE(ssb_ref.is_process_called());
  ASSERT_FALSE(csi_rs_ref.is_map_called());
  ASSERT_FALSE(gw.sent);

  dl_processor->process_ssb({});
  ASSERT_TRUE(ssb_ref.is_process_called());

  pdcch_processor::pdu_t pdu;
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  ASSERT_TRUE(pdcch_ref.is_process_called());

  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  ASSERT_TRUE(pdsch_ref.is_process_called());

  dl_processor->process_nzp_csi_rs({});
  ASSERT_TRUE(csi_rs_ref.is_map_called());

  ASSERT_FALSE(gw.sent);

  dl_processor->finish_processing_pdus();

  ASSERT_TRUE(gw.sent);
}

TEST(downlinkProcessorTest, finishIsCalledBeforeProcessingPdus)
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
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});

  ASSERT_FALSE(pdcch_ref.is_process_called());
  ASSERT_FALSE(pdsch_ref.is_process_called());
  ASSERT_FALSE(ssb_ref.is_process_called());
  ASSERT_FALSE(csi_rs_ref.is_map_called());
  ASSERT_FALSE(gw.sent);

  dl_processor->finish_processing_pdus();
  ASSERT_FALSE(gw.sent);

  // Run all the queued tasks.
  executor.run_pending_tasks();

  ASSERT_TRUE(pdcch_ref.is_process_called());
  ASSERT_TRUE(pdsch_ref.is_process_called());
  ASSERT_TRUE(ssb_ref.is_process_called());
  ASSERT_TRUE(csi_rs_ref.is_map_called());

  ASSERT_TRUE(gw.sent);
}

TEST(downlinkProcessorTest, processPduAfterFinishProcessingPdusDoesNothing)
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
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->finish_processing_pdus();

  ASSERT_TRUE(pdcch_ref.is_process_called());
  ASSERT_TRUE(pdsch_ref.is_process_called());
  ASSERT_TRUE(ssb_ref.is_process_called());
  ASSERT_TRUE(gw.sent);

  // Process a PDU after finish_processing_pdus() method has been called.
  {
    bool processed = dl_processor->process_nzp_csi_rs({});
    ASSERT_FALSE(processed);
  }
  ASSERT_FALSE(csi_rs_ref.is_map_called());
}

TEST(downlinkProcessorTest, processPduBeforeConfigureDoesNothing)
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
  pdu.dci.precoding         = precoding_configuration::make_wideband(make_single_port());
  std::vector<uint8_t> data = {1, 2, 3, 4};

  {
    bool processed = dl_processor->process_pdcch(pdu);
    ASSERT_FALSE(processed);
  }
  {
    bool processed = dl_processor->process_pdsch({data}, {});
    ASSERT_FALSE(processed);
  }
  {
    bool processed = dl_processor->process_nzp_csi_rs({});
    ASSERT_FALSE(processed);
  }

  ASSERT_FALSE(pdcch_ref.is_process_called());
  ASSERT_FALSE(pdsch_ref.is_process_called());
  ASSERT_FALSE(ssb_ref.is_process_called());
  ASSERT_FALSE(csi_rs_ref.is_map_called());
  ASSERT_FALSE(gw.sent);
}

TEST(downlinkProcessorTest, finishBeforeConfigureDeath)
{
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto dl_processor =
      std::make_unique<downlink_processor_single_executor_impl>(gw,
                                                                std::make_unique<pdcch_processor_spy>(),
                                                                std::make_unique<pdsch_processor_spy>(),
                                                                std::make_unique<ssb_processor_spy>(),
                                                                std::make_unique<csi_rs_processor_spy>(),
                                                                executor);

  ASSERT_TRUE(!gw.sent);

#ifdef ASSERTS_ENABLED
  ASSERT_DEATH({ dl_processor->finish_processing_pdus(); },
               R"(DL processor finish was requested in an invalid state\, i\.e\.\, idle.)");
#endif // ASSERTS_ENABLED

  ASSERT_TRUE(!gw.sent);
}

TEST(downlinkProcessorTest, twoConsecutiveSlots)
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

  resource_grid_dummy grid;
  dl_processor->configure_resource_grid({slot, sector}, grid);

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});
  ASSERT_TRUE(!gw.sent);

  dl_processor->finish_processing_pdus();

  ASSERT_TRUE(gw.sent);

  slot_point slot2(1, 2, 2);
  gw.clear_sent();
  resource_grid_dummy grid2;
  dl_processor->configure_resource_grid({slot2, sector}, grid2);

  dl_processor->process_ssb({});
  dl_processor->process_pdcch(pdu);
  dl_processor->process_pdsch({data}, {});
  dl_processor->process_nzp_csi_rs({});
  ASSERT_FALSE(gw.sent);

  dl_processor->finish_processing_pdus();

  ASSERT_TRUE(gw.sent);
}

TEST(downlinkProcessorTest, finishWithoutProcessingPdusSendsTheGrid)
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

  resource_grid_spy grid(0, 0, 0);
  dl_processor->configure_resource_grid({slot, sector}, grid);

  // The resource grid should not have been set to zero yet.
  ASSERT_FALSE(grid.has_set_all_zero_method_been_called());

  // The resource grid set all zero should be enqueued.
  ASSERT_TRUE(executor.has_pending_tasks());

  // Run resource grid zero set.
  executor.run_pending_tasks();

  // The resource grid set all zero should have been called.
  ASSERT_TRUE(grid.has_set_all_zero_method_been_called());

  ASSERT_FALSE(gw.sent);

  // By finishing PDUs, the resource grid should be sent.
  dl_processor->finish_processing_pdus();

  ASSERT_FALSE(executor.has_pending_tasks());
  ASSERT_TRUE(gw.sent);
}
