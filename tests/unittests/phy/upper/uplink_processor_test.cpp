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
#include "signal_processors/srs/srs_estimator_test_doubles.h"
#include "upper_phy_rx_results_notifier_test_doubles.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

class rx_buffer_pool_spy : public rx_buffer_pool, private unique_rx_buffer::callback
{
public:
  unique_rx_buffer
  reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data) override
  {
    if (is_locked()) {
      return unique_rx_buffer();
    }
    return unique_rx_buffer(*this);
  }
  void run_slot(const slot_point& slot) override { current_slot = slot; }

  slot_point get_current_slot() const { return current_slot; }

  bool is_locked() const { return is_buffer_locked; }

  unsigned                   get_nof_codeblocks() const override { return 0; }
  void                       reset_codeblocks_crc() override {}
  span<bool>                 get_codeblocks_crc() override { return {}; }
  unsigned                   get_absolute_codeblock_id(unsigned codeblock_id) const override { return 0; }
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    return {};
  }
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    static static_bit_buffer<8> buffer = {};
    return buffer;
  }

  bool try_lock() override
  {
    is_buffer_locked = true;
    return true;
  }
  void unlock() override { is_buffer_locked = false; }
  void release() override { is_buffer_locked = false; }

  void clear()
  {
    current_slot     = {};
    is_buffer_locked = false;
  }

private:
  slot_point current_slot     = {};
  bool       is_buffer_locked = false;
};

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
    grid_spy(grid_reader_spy, grid_writer_spy),
    grid(grid_spy)
  {
  }

  void SetUp() override
  {
    auto prach      = std::make_unique<prach_detector_spy>();
    auto pusch_proc = std::make_unique<pusch_processor_spy>();
    auto pucch_proc = std::make_unique<pucch_processor_dummy>();
    auto srs        = std::make_unique<srs_estimator_dummy>();

    ASSERT_NE(prach, nullptr);
    ASSERT_NE(pusch_proc, nullptr);
    ASSERT_NE(pucch_proc, nullptr);
    ASSERT_NE(srs, nullptr);

    prach_spy = prach.get();
    pusch_spy = pusch_proc.get();

    uplink_processor_impl::task_executor_collection executors = {.pucch_executor = pucch_executor,
                                                                 .pusch_executor = pusch_executor,
                                                                 .srs_executor   = srs_executor,
                                                                 .prach_executor = prach_executor};

    ul_processor = std::make_unique<uplink_processor_impl>(std::move(prach),
                                                           std::move(pusch_proc),
                                                           std::move(pucch_proc),
                                                           std::move(srs),
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
  resource_grid_spy        grid_spy;
  shared_resource_grid_spy grid;
};

TEST_F(UplinkProcessorFixture, prach_workflow)
{
  slot_point slot;
  ul_processor->get_pdu_slot_repository(slot);

  prach_buffer_spy buffer;
  ul_processor->get_slot_processor(slot).process_prach(buffer, {});

  // Check PRACH detection has been enqueued and the detector was not called.
  ASSERT_TRUE(prach_executor.has_pending_tasks());
  ASSERT_FALSE(prach_spy->has_detect_method_been_called());

  // Execute tasks.
  prach_executor.run_pending_tasks();

  // Check the detector has been called and the result notified.
  ASSERT_TRUE(prach_spy->has_detect_method_been_called());
  ASSERT_TRUE(results_notifier.has_prach_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_normal_workflow)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  shared_resource_grid shared_grid = grid.get_grid();

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index - 1);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

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

  shared_resource_grid shared_grid = grid.get_grid();

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index - 1);

  // Notify reception of the symbol before the previous.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index - 2);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

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

  shared_resource_grid shared_grid = grid.get_grid();

  // Notify reception of receive symbol for the wrong slot.
  ul_processor->get_slot_processor(slot - 1).handle_rx_symbol(shared_grid, end_symbol_index);

  // Check PUSCH processing has NOT been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol for the right slot.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

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

  shared_resource_grid shared_grid = grid.get_grid();

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

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

  shared_resource_grid shared_grid = grid.get_grid();

  // Lock buffer pool to ensure it fails to retrieve a buffer.
  ASSERT_TRUE(buffer_pool_spy.try_lock());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

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

  shared_resource_grid shared_grid = grid.get_grid();

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_discard_slot)
{
  // Contexted slot.
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
  // Contexted slot.
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pucch_pdu(pucch_pdu);
  }

  shared_resource_grid shared_grid = grid.get_grid();

  // Discard.
  std::thread async1 = std::thread([this, slot]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  std::thread async2 = std::thread([this, slot]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  async1.join();
  async2.join();

  // Assert results.
  ASSERT_FALSE(pucch_executor.has_pending_tasks());
  ASSERT_TRUE(results_notifier.has_pucch_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_execute_after_stop)
{
  slot_point slot;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  shared_resource_grid shared_grid = grid.get_grid();

  // Get processor before stopping. Otherwise, the processor will not be available.
  uplink_slot_processor& processor = ul_processor->get_slot_processor(slot);

  // Stop processor.
  ul_processor->stop();

  // Notify reception of receive symbol.
  processor.handle_rx_symbol(shared_grid, end_symbol_index);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_without_request)
{
  // Contexted slot.
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
  // Contexted slot.
  slot_point slot = pusch_pdu.pdu.slot;

  // Get the repository for the first time - it has a pending PDU.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
    ASSERT_TRUE(repository.is_valid());
  }

  // Get the reporsitory for the second time - it shall return an invalid repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());
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
  // Contexted slot.
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
  // Contexted slot.
  slot_point slot = pusch_pdu.pdu.slot;

  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Notify reception of receive symbol - an asynchronous task is expected.
  unsigned             end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;
  shared_resource_grid shared_grid      = grid.get_grid();
  ul_processor->get_slot_processor(slot).handle_rx_symbol(shared_grid, end_symbol_index);
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
  // Contexted slot.
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
