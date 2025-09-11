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

#include "../../../lib/phy/upper/downlink_processor_multi_executor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "channel_processors/pdcch/pdcch_processor_test_doubles.h"
#include "channel_processors/pdsch/pdsch_processor_test_doubles.h"
#include "channel_processors/ssb/ssb_processor_test_doubles.h"
#include "signal_processors/nzp_csi_rs_generator_test_doubles.h"
#include "signal_processors/prs/prs_generator_test_doubles.h"
#include "upper_phy_rg_gateway_test_doubles.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "gtest/gtest.h"

using namespace srsran;

static srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY");

static shared_resource_grid get_dummy_grid()
{
  static resource_grid_reader_spy rg_reader_spy;
  static resource_grid_writer_spy rg_writer_spy;
  static resource_grid_spy        rg_spy(rg_reader_spy, rg_writer_spy);
  static shared_resource_grid_spy unique_rg_spy(rg_spy);
  return unique_rg_spy.get_grid();
}

TEST(downlinkProcessorTest, worksInOrder)
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  auto pdcch_processor  = std::make_unique<pdcch_processor_spy>();
  auto pdsch_processor  = std::make_unique<pdsch_processor_spy>();
  auto ssb_processor    = std::make_unique<ssb_processor_spy>();
  auto csi_rs_processor = std::make_unique<csi_rs_processor_spy>();
  auto prs_generator    = std::make_unique<prs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref   = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref   = *pdsch_processor;
  ssb_processor_spy&    ssb_ref     = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref  = *csi_rs_processor;
  prs_processor_spy&    prs_gen_ref = *prs_generator;

  std::unique_ptr<downlink_processor_base> dl_proc_base =
      std::make_unique<downlink_processor_multi_executor_impl>(gw,
                                                               std::move(pdcch_processor),
                                                               std::move(pdsch_processor),
                                                               std::move(ssb_processor),
                                                               std::move(csi_rs_processor),
                                                               std::move(prs_generator),
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               logger);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  downlink_processor_controller& dl_proc_control = dl_proc_base->get_controller();
  unique_downlink_processor dl_processor = dl_proc_control.configure_resource_grid({slot, sector}, get_dummy_grid());
  ASSERT_TRUE(dl_processor.is_valid());

  ASSERT_FALSE(pdcch_ref.is_process_called());
  ASSERT_FALSE(pdsch_ref.is_process_called());
  ASSERT_FALSE(ssb_ref.is_process_called());
  ASSERT_FALSE(csi_rs_ref.is_map_called());
  ASSERT_FALSE(prs_gen_ref.is_generate_called());
  ASSERT_FALSE(gw.sent);

  dl_processor->process_ssb({});

  pdcch_processor::pdu_t pdu;
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);

  std::vector<uint8_t> pdsch_data = {1, 2, 3, 4};
  dl_processor->process_pdsch({shared_transport_block(pdsch_data)}, {});
  dl_processor->process_nzp_csi_rs({});

  unsigned task_count = 0;
  while (executor.try_run_next()) {
    ++task_count;
  }
  ASSERT_EQ(task_count, 4);

  ASSERT_TRUE(ssb_ref.is_process_called());
  ASSERT_TRUE(pdcch_ref.is_process_called());
  ASSERT_TRUE(pdsch_ref.is_process_called());
  ASSERT_TRUE(csi_rs_ref.is_map_called());

  ASSERT_FALSE(gw.sent);

  dl_processor.release();

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
  auto prs_generator    = std::make_unique<prs_processor_spy>();

  pdcch_processor_spy&  pdcch_ref   = *pdcch_processor;
  pdsch_processor_spy&  pdsch_ref   = *pdsch_processor;
  ssb_processor_spy&    ssb_ref     = *ssb_processor;
  csi_rs_processor_spy& csi_rs_ref  = *csi_rs_processor;
  prs_processor_spy&    prs_gen_ref = *prs_generator;

  std::unique_ptr<downlink_processor_base> dl_proc_base =
      std::make_unique<downlink_processor_multi_executor_impl>(gw,
                                                               std::move(pdcch_processor),
                                                               std::move(pdsch_processor),
                                                               std::move(ssb_processor),
                                                               std::move(csi_rs_processor),
                                                               std::move(prs_generator),
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               logger);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  downlink_processor_controller& dl_proc_control = dl_proc_base->get_controller();
  unique_downlink_processor dl_processor = dl_proc_control.configure_resource_grid({slot, sector}, get_dummy_grid());
  ASSERT_TRUE(dl_processor.is_valid());

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({shared_transport_block(data)}, {});
  dl_processor->process_nzp_csi_rs({});
  dl_processor->process_prs({});

  ASSERT_FALSE(pdcch_ref.is_process_called());
  ASSERT_FALSE(pdsch_ref.is_process_called());
  ASSERT_FALSE(ssb_ref.is_process_called());
  ASSERT_FALSE(csi_rs_ref.is_map_called());
  ASSERT_FALSE(prs_gen_ref.is_generate_called());
  ASSERT_FALSE(gw.sent);

  dl_processor.release();
  ASSERT_FALSE(gw.sent);

  // Run all the queued tasks.
  executor.run_pending_tasks();

  ASSERT_TRUE(pdcch_ref.is_process_called());
  ASSERT_TRUE(pdsch_ref.is_process_called());
  ASSERT_TRUE(ssb_ref.is_process_called());
  ASSERT_TRUE(csi_rs_ref.is_map_called());
  ASSERT_TRUE(prs_gen_ref.is_generate_called());

  ASSERT_TRUE(gw.sent);
}

TEST(downlinkProcessorTest, twoConsecutiveSlots)
{
  upper_phy_rg_gateway_fto gw;
  manual_task_worker       executor(10);

  std::unique_ptr<downlink_processor_base> dl_proc_base =
      std::make_unique<downlink_processor_multi_executor_impl>(gw,
                                                               std::make_unique<pdcch_processor_spy>(),
                                                               std::make_unique<pdsch_processor_spy>(),
                                                               std::make_unique<ssb_processor_spy>(),
                                                               std::make_unique<csi_rs_processor_spy>(),
                                                               std::make_unique<prs_processor_spy>(),
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               logger);

  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy            grid;
  downlink_processor_controller& dl_proc_control = dl_proc_base->get_controller();
  unique_downlink_processor dl_processor = dl_proc_control.configure_resource_grid({slot, sector}, get_dummy_grid());
  ASSERT_TRUE(dl_processor.is_valid());

  dl_processor->process_ssb({});
  pdcch_processor::pdu_t pdu;
  pdu.dci.precoding = precoding_configuration::make_wideband(make_single_port());
  dl_processor->process_pdcch(pdu);
  std::vector<uint8_t> data = {1, 2, 3, 4};
  dl_processor->process_pdsch({shared_transport_block(data)}, {});
  dl_processor->process_nzp_csi_rs({});
  ASSERT_TRUE(!gw.sent);

  dl_processor.release();
  while (executor.try_run_next()) {
  }
  ASSERT_TRUE(gw.sent);

  slot_point slot2(1, 2, 2);
  gw.clear_sent();
  dl_processor = dl_proc_control.configure_resource_grid({slot2, sector}, get_dummy_grid());
  ASSERT_TRUE(dl_processor.is_valid());

  dl_processor->process_ssb({});
  dl_processor->process_pdcch(pdu);
  dl_processor->process_pdsch({shared_transport_block(data)}, {});
  dl_processor->process_nzp_csi_rs({});
  ASSERT_FALSE(gw.sent);

  dl_processor.release();

  ASSERT_FALSE(gw.sent);
  while (executor.try_run_next()) {
  }

  ASSERT_TRUE(gw.sent);
}

TEST(downlinkProcessorTest, finishWithoutProcessingPdusSendsTheGrid)
{
  upper_phy_rg_gateway_fto                gw;
  manual_task_worker_always_enqueue_tasks executor(10);

  std::unique_ptr<downlink_processor_base> dl_proc_base =
      std::make_unique<downlink_processor_multi_executor_impl>(gw,
                                                               std::make_unique<pdcch_processor_spy>(),
                                                               std::make_unique<pdsch_processor_spy>(),
                                                               std::make_unique<ssb_processor_spy>(),
                                                               std::make_unique<csi_rs_processor_spy>(),
                                                               std::make_unique<prs_processor_spy>(),
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               executor,
                                                               logger);
  slot_point slot(1, 2, 1);
  unsigned   sector = 0;

  resource_grid_dummy            grid;
  downlink_processor_controller& dl_proc_control = dl_proc_base->get_controller();
  unique_downlink_processor dl_processor = dl_proc_control.configure_resource_grid({slot, sector}, get_dummy_grid());
  ASSERT_TRUE(dl_processor.is_valid());

  // By finishing PDUs, the resource grid should be sent.
  dl_processor.release();

  ASSERT_FALSE(executor.has_pending_tasks());
  ASSERT_TRUE(gw.sent);
}
