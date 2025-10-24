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

#include "../../../lib/phy/upper/uplink_processor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/prach_buffer_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "channel_processors/prach_detector_test_doubles.h"
#include "channel_processors/pucch/pucch_processor_test_doubles.h"
#include "channel_processors/pusch/pusch_processor_test_doubles.h"
#include "rx_buffer_pool_test_doubles.h"
#include "signal_processors/srs/srs_estimator_test_doubles.h"
#include "upper_phy_rx_results_notifier_test_doubles.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

class UplinkProcessorFixture : public ::testing::Test
{
public:
  UplinkProcessorFixture() :
    pucch_executor(1),
    pusch_executor(1),
    srs_executor(1),
    prach_executor(1),
    grid_reader_spy(max_nof_layers, max_nof_symbols, max_nof_prb),
    grid_writer_spy(max_nof_layers, max_nof_symbols, max_nof_prb),
    grid_spy(nullptr)
  {
  }

  void SetUp() override
  {
    auto prach      = std::make_unique<prach_detector_spy>();
    auto pusch_proc = std::make_unique<pusch_processor_spy>();
    auto pucch_proc = std::make_unique<pucch_processor_dummy>();
    auto srs        = std::make_unique<srs_estimator_dummy>();
    auto grid       = std::make_unique<resource_grid_spy>(grid_reader_spy, grid_writer_spy);

    ASSERT_NE(prach, nullptr);
    ASSERT_NE(pusch_proc, nullptr);
    ASSERT_NE(pucch_proc, nullptr);
    ASSERT_NE(srs, nullptr);

    prach_spy = prach.get();
    pusch_spy = pusch_proc.get();
    grid_spy  = grid.get();

    uplink_processor_impl::task_executor_collection executors = {.pucch_executor = pucch_executor,
                                                                 .pusch_executor = pusch_executor,
                                                                 .srs_executor   = srs_executor,
                                                                 .prach_executor = prach_executor};

    ul_processor = std::make_unique<uplink_processor_impl>(std::move(prach),
                                                           std::move(pusch_proc),
                                                           std::move(pucch_proc),
                                                           std::move(srs),
                                                           std::move(grid),
                                                           std::unique_ptr<phy_tap>(),
                                                           executors,
                                                           buffer_pool_spy,
                                                           results_notifier,
                                                           max_nof_prb,
                                                           max_nof_layers);

    buffer_pool_spy.clear();
  }

protected:
  static constexpr unsigned max_nof_prb     = 15;
  static constexpr unsigned max_nof_layers  = 1;
  static constexpr unsigned max_nof_symbols = 14;

  const uplink_pdu_slot_repository::pusch_pdu pusch_pdu = {
      .harq_id = 0,
      .tb_size = units::bytes(8),
      .pdu     = {.context       = std::nullopt,
                  .slot          = {0, 9},
                  .rnti          = 8323,
                  .bwp_size_rb   = 25,
                  .bwp_start_rb  = 0,
                  .cp            = cyclic_prefix::NORMAL,
                  .mcs_descr     = {modulation_scheme::PI_2_BPSK, 0.1},
                  .codeword      = {{0, ldpc_base_graph_type::BG2, true}},
                  .uci           = {1, 0, uci_part2_size_description(1), 1, 20, 6.25, 6.25},
                  .n_id          = 935,
                  .nof_tx_layers = 1,
                  .rx_ports      = {0, 1, 2, 3},
                  .dmrs_symbol_mask =
                      {false, false, true, false, false, false, false, false, false, false, false, true, false, false},
                  .dmrs               = pusch_processor::dmrs_configuration{.dmrs                        = dmrs_type::TYPE1,
                                                                            .scrambling_id               = 0,
                                                                            .n_scid                      = false,
                                                                            .nof_cdm_groups_without_data = 2},
                  .freq_alloc         = rb_allocation::make_type1(15, 1),
                  .start_symbol_index = 0,
                  .nof_symbols        = 14,
                  .tbs_lbrm           = units::bytes(ldpc::MAX_CODEBLOCK_SIZE / 8),
                  .dc_position        = std::nullopt}};

  const uplink_pdu_slot_repository::pucch_pdu pucch_pdu = {
      .context = {.slot          = {0, 9},
                  .rnti          = to_rnti(0x4601),
                  .format        = pucch_format::FORMAT_0,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format0_configuration{.context              = std::nullopt,
                                                        .slot                 = {0, 9},
                                                        .cp                   = cyclic_prefix::NORMAL,
                                                        .bwp_size_rb          = 275,
                                                        .bwp_start_rb         = 0,
                                                        .starting_prb         = 0,
                                                        .second_hop_prb       = 270,
                                                        .start_symbol_index   = 0,
                                                        .nof_symbols          = 1,
                                                        .initial_cyclic_shift = 0,
                                                        .n_id                 = 0,
                                                        .nof_harq_ack         = 1,
                                                        .sr_opportunity       = true,
                                                        .ports                = {0}}};

  prach_detector_spy*                     prach_spy = nullptr;
  pusch_processor_spy*                    pusch_spy = nullptr;
  manual_task_worker_always_enqueue_tasks pucch_executor;
  manual_task_worker_always_enqueue_tasks pusch_executor;
  manual_task_worker_always_enqueue_tasks srs_executor;
  manual_task_worker_always_enqueue_tasks prach_executor;
  rx_buffer_pool_spy                      buffer_pool_spy;
  upper_phy_rx_results_notifier_spy       results_notifier;
  std::unique_ptr<uplink_processor_impl>  ul_processor;

  resource_grid_reader_spy grid_reader_spy;
  resource_grid_writer_spy grid_writer_spy;
  resource_grid_spy*       grid_spy;
};

TEST_F(UplinkProcessorFixture, prach_normal_workflow)
{
  slot_point slot;
  ul_processor->get_pdu_slot_repository(slot);

  // Request PRACH processing.
  prach_buffer_spy buffer;
  ul_processor->get_slot_processor(slot).process_prach(buffer, {});

  // Create asynchronous task - it will block until all tasks are completed.
  std::atomic<bool> stop_thread_started = false;
  std::thread       stop_thread([this, &stop_thread_started]() {
    stop_thread_started = true;
    ul_processor->stop();
  });

  // Wait for thread to start.
  while (!stop_thread_started) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // Execute tasks.
  ASSERT_TRUE(prach_executor.run_pending_tasks());

  // Check the detector has been called and the result notified.
  ASSERT_TRUE(prach_spy->has_detect_method_been_called());
  ASSERT_TRUE(results_notifier.has_prach_result_been_notified());

  // Synchronize stopping thread.
  stop_thread.join();
}

TEST_F(UplinkProcessorFixture, prach_fail_defer_workflow)
{
  slot_point slot;
  ul_processor->get_pdu_slot_repository(slot);

  // Stop PRACH executor.
  prach_executor.stop();

  // Request PRACH processing.
  prach_buffer_spy buffer;
  ul_processor->get_slot_processor(slot).process_prach(buffer, {});

  // Request processor to stop. There is no pending task, so it should not block.
  ul_processor->stop();

  // Check the detector has not been called nor the result notified.
  ASSERT_FALSE(prach_spy->has_detect_method_been_called());
  ASSERT_FALSE(results_notifier.has_prach_result_been_notified());
}

TEST_F(UplinkProcessorFixture, prach_request_after_stop)
{
  slot_point slot;
  ul_processor->get_pdu_slot_repository(slot);

  // Request processor to stop. There is no pending task, so it should not block.
  ul_processor->stop();

  // Request PRACH processing.
  prach_buffer_spy buffer;
  ul_processor->get_slot_processor(slot).process_prach(buffer, {});

  // The UL processor must not create an asynchronous task.
  ASSERT_FALSE(prach_executor.run_pending_tasks());

  // Check the detector has not been called nor the result notified.
  ASSERT_FALSE(prach_spy->has_detect_method_been_called());
  ASSERT_FALSE(results_notifier.has_prach_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_normal_workflow)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 1, true);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, rx_symbol_bad_order)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 1, true);

  // Notify reception of the symbol before the previous.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 2, true);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_wrong_slot)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of receive symbol for the wrong slot.
  ul_processor->get_slot_processor(slot - 1).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has NOT been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol for the right slot.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_exceed_tb_size)
{
  // Create PDU with a larger TB than the one the pool contains.
  uplink_pdu_slot_repository::pusch_pdu pusch_pdu_large = pusch_pdu;
  pusch_pdu_large.tb_size                               = units::bytes(2 * max_nof_layers * 156 * max_nof_prb);

  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu_large);
  }

  unsigned end_symbol_index = pusch_pdu_large.pdu.start_symbol_index + pusch_pdu_large.pdu.nof_symbols - 1;

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has not been called and the result notified.
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_locked_rx_buffer)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Lock buffer pool to ensure it fails to retrieve a buffer.
  ASSERT_TRUE(buffer_pool_spy.try_lock());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_fail_executor)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Ensure PUSCH executor does not enqueue more tasks.
  pusch_executor.stop();

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_discard_slot)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Discard.
  ul_processor->get_slot_processor(slot).discard_slot();

  // Assert results.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pucch_discard_twice)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pucch_pdu(pucch_pdu);
  }

  // Discard.
  std::thread async1 = std::thread([this, slot]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  std::thread async2 = std::thread([this, slot]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  async1.join();
  async2.join();

  // Assert results.
  ASSERT_FALSE(pucch_executor.has_pending_tasks());
  ASSERT_TRUE(results_notifier.has_pucch_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_discard_invalid_symbol)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify all symbols as valid except for the last one.
  for (unsigned i_symbol = 0; i_symbol != end_symbol_index - 1; ++i_symbol) {
    ul_processor->get_slot_processor(slot).handle_rx_symbol(i_symbol, true);
  }
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 1, false);

  // Assert PDU was discarded.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());

  // Clear notifier spy, feed last OFDM symbol and make sure nothing was notified.
  results_notifier.clear();
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_execute_after_stop)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Get processor before stopping. Otherwise, the processor will not be available.
  uplink_slot_processor& processor = ul_processor->get_slot_processor(slot);

  // Stop processor.
  ul_processor->stop();

  // Notify reception of receive symbol.
  processor.handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_without_request)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Get the repository for the first time - it transitions to idle as there is no request.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    ASSERT_TRUE(repository.is_valid());
  }

  // Get the reporsitory for the second time - it shall return a valid repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_with_request)
{
  // Context slot.
  slot_point slot = pusch_pdu.pdu.slot;

  // Get the repository for the first time - it has a pending PDU.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
    ASSERT_TRUE(repository.is_valid());
  }

  // Get the repository for the second time - it shall return a valid repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_pending_exec)
{
  // Context slot.
  slot_point slot = pusch_pdu.pdu.slot;

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Get the repository for the first time - it has a pending PDU.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
    ASSERT_TRUE(repository.is_valid());
  }

  // Notify reception of receive symbol.
  {
    ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  }

  // Assert execution expectations.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());

  // Get the repository for the second time - it shall return an invalid repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());

  ASSERT_TRUE(pusch_executor.run_pending_tasks());
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());

  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());
}

TEST_F(UplinkProcessorFixture, stop_no_pending_task)
{
  // Direct stop.
  ul_processor->stop();

  // Assert results.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, stop_accepting_tasks)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);

  // Create asynchronous task - it will block until the repository is released.
  std::atomic<bool> stop_thread_running = false;
  std::thread       stop_thread([this, &stop_thread_running]() {
    stop_thread_running = true;
    ul_processor->stop();
  });

  // Wait for the thread to start - facilitates that the stop method blocks until the state is released.
  while (!stop_thread_running.load()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  // Release repository - the stop method shall return.
  repository.release();

  // Synchronize stopping thread.
  stop_thread.join();

  // Assert execution expectations.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, stop_pending_task)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Notify reception of receive symbol - an asynchronous task is expected.
  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_TRUE(pusch_executor.has_pending_tasks());

  // Create asynchronous task - it will block until all tasks are completed.
  std::thread stop_thread([this]() { ul_processor->stop(); });

  // Execute PUSCH asynchronous task.
  pusch_executor.try_run_next();

  // Assert execution expectations.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());

  // Synchronize stopping thread.
  stop_thread.join();
}

TEST_F(UplinkProcessorFixture, pusch_discard_slot_after_stop)
{
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Stop.
  ul_processor->stop();

  // Discard.
  ul_processor->get_slot_processor(slot).discard_slot();

  // Assert results.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
}

} // namespace
