/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../support/task_executor_test_doubles.h"
#include "../../../support/prach_buffer_test_doubles.h"
#include "../../modulation/ofdm_prach_demodulator_test_doubles.h"
#include "prach_processor_test_doubles.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_baseband.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_factories.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_request_handler.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include <gtest/gtest.h>
#include <numeric>

namespace srsgnb {
bool operator==(const prach_buffer_context& rhs, const prach_buffer_context& lhs)
{
  return std::memcmp(&rhs, &lhs, sizeof(prach_buffer_context)) == 0;
}
} // namespace srsgnb

using namespace srsgnb;

// Combined parameters. First the DFT size for 15kHz and second the maximum number of concurrent PRACH requests.
using prach_processor_param = std::tuple<unsigned, unsigned>;

static constexpr slot_point TEST_SLOT_BEGIN   = slot_point(0, 0);
static constexpr slot_point TEST_SLOT_END     = slot_point(0, 10);
static constexpr unsigned   TEST_SYMBOL_BEGIN = 0;
static constexpr unsigned   TEST_SYMBOL_END   = MAX_NSYMB_PER_SLOT;

class prach_processor_test : public ::testing::TestWithParam<prach_processor_param>
{
protected:
  std::shared_ptr<ofdm_prach_demodulator_factory_spy> ofdm_prach_factory_spy;
  std::unique_ptr<prach_processor>                    processor;
  manual_task_worker_always_enqueue_tasks             task_executor_spy;
  prach_processor_notifier_spy                        notifier_spy;

  prach_processor_test() : task_executor_spy(1), notifier_spy("none")
  {
    // Do nothing.
  }

  ~prach_processor_test() = default;

  void SetUp() override
  {
    Test::SetUp();

    unsigned dft_size_15kHz              = std::get<0>(GetParam());
    unsigned max_nof_concurrent_requests = std::get<1>(GetParam());

    // Create OFDM PRACH demodulator spy.
    ofdm_prach_factory_spy = std::make_shared<ofdm_prach_demodulator_factory_spy>();

    // Create factory.
    std::shared_ptr<prach_processor_factory> factory =
        create_prach_processor_factory_sw(ofdm_prach_factory_spy, dft_size_15kHz, max_nof_concurrent_requests);
    ASSERT_TRUE(factory);

    // Create processor.
    processor = factory->create(task_executor_spy);
    ASSERT_TRUE(processor);

    // Connect processor with the notifier.
    processor->connect(notifier_spy);

    // Validate that the number of OFDM PRACH demodulators match with the maximum number of concurrent requests.
    ASSERT_EQ(ofdm_prach_factory_spy->get_entries().size(), max_nof_concurrent_requests);

    // Clear notifications.
    notifier_spy.clear_notifications();
  }
};

// If there is no request, the PRACH processor shall:
// - not notify any event,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(prach_processor_test, idle)
{
  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned symbol = TEST_SYMBOL_BEGIN; symbol != TEST_SYMBOL_END; ++symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = symbol;
      symbol_context.sector = 0;
      symbol_context.port   = 0;

      processor->get_baseband().process_symbol(span<cf_t>(), symbol_context);
    }
  }

  // Verify no entity was used.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// If the sector does not match, the PRACH processor shall:
// - not use the buffer methods,
// - not notify any event,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(prach_processor_test, sector_unmatch)
{
  unsigned sector       = 0;
  unsigned rogue_sector = 1;

  prach_buffer_context context;
  context.sector       = rogue_sector;
  context.port         = 0;
  context.slot         = TEST_SLOT_END;
  context.start_symbol = 0;
  context.format       = preamble_format::FORMAT0;
  context.rb_offset    = 0;
  context.pusch_scs    = subcarrier_spacing::kHz15;

  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned symbol = TEST_SYMBOL_BEGIN; symbol != TEST_SYMBOL_END; ++symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = symbol;
      symbol_context.sector = sector;
      symbol_context.port   = 0;

      processor->get_baseband().process_symbol(span<cf_t>(), symbol_context);
    }
  }

  // Verify no entity was used.
  ASSERT_EQ(0, buffer.get_total_count());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// If the slot in then request is late, the PRACH processor shall:
// - notify a PRACH request late event in slot `context.slot + 1`,
// - not use the buffer methods,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(prach_processor_test, detect_late)
{
  prach_buffer_context context;
  context.sector       = 0;
  context.port         = 0;
  context.slot         = TEST_SLOT_BEGIN;
  context.start_symbol = 0;
  context.format       = preamble_format::FORMAT0;
  context.rb_offset    = 0;
  context.pusch_scs    = subcarrier_spacing::kHz15;

  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = context.slot + 1; slot != TEST_SLOT_END; ++slot) {
    for (unsigned symbol = TEST_SYMBOL_BEGIN; symbol != TEST_SYMBOL_END; ++symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = symbol;
      symbol_context.sector = 0;
      symbol_context.port   = 0;

      processor->get_baseband().process_symbol(span<cf_t>(), symbol_context);

      if (slot == context.slot + 1 && symbol == 0) {
        ASSERT_EQ(1, notifier_spy.get_nof_notifications());

        ASSERT_EQ(1, notifier_spy.get_request_late_entries().size());
        const auto& late_entry = notifier_spy.get_request_late_entries().back();
        ASSERT_EQ(late_entry, context);
        notifier_spy.clear_notifications();
      }
    }
  }

  // Verify no entity was used.
  ASSERT_EQ(0, buffer.get_total_count());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// If the slot in then request is late and the sector does not match, the PRACH processor shall:
// - notify a PRACH request late event in `context.slot + 1`,
// - not use the buffer methods,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(prach_processor_test, detect_unmatch_sector_late)
{
  unsigned sector       = 0;
  unsigned rogue_sector = 1;

  prach_buffer_context context;
  context.sector       = rogue_sector;
  context.port         = 0;
  context.slot         = TEST_SLOT_BEGIN;
  context.start_symbol = 0;
  context.format       = preamble_format::FORMAT0;
  context.rb_offset    = 0;
  context.pusch_scs    = subcarrier_spacing::kHz15;

  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned symbol = TEST_SYMBOL_BEGIN; symbol != TEST_SYMBOL_END; ++symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = symbol;
      symbol_context.sector = sector;
      symbol_context.port   = 0;

      processor->get_baseband().process_symbol(span<cf_t>(), symbol_context);

      if (slot == context.slot + 1 && symbol == 0) {
        ASSERT_EQ(1, notifier_spy.get_nof_notifications());

        ASSERT_EQ(1, notifier_spy.get_request_late_entries().size());
        const auto& late_entry = notifier_spy.get_request_late_entries().back();
        ASSERT_EQ(late_entry, context);
        notifier_spy.clear_notifications();
      }
    }
  }

  // Verify no entity was used.
  ASSERT_EQ(0, buffer.get_total_count());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// If more than `max_nof_concurrent_requests` PRACH request are requested, the PRACH processor shall:
// - notify that the contexts that exceed the maximum number of requests have been dropped,
// - not use the buffer methods,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(prach_processor_test, overflow)
{
  unsigned max_nof_concurrent_requests = std::get<1>(GetParam());

  // For the maximum number of concurrent requests...
  for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
    // Prepare context with the count as sector.
    prach_buffer_context context;
    context.sector       = count;
    context.port         = 0;
    context.slot         = TEST_SLOT_BEGIN;
    context.start_symbol = 0;
    context.format       = preamble_format::FORMAT0;
    context.rb_offset    = 0;
    context.pusch_scs    = subcarrier_spacing::kHz15;

    prach_buffer_spy buffer;

    // Do request.
    processor->get_request_handler().handle_request(buffer, context);
  }

  // No notification was expected.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // For the maximum number of concurrent requests...
  for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
    // Prepare context with the count as sector.
    prach_buffer_context context;
    context.sector       = count + max_nof_concurrent_requests;
    context.port         = 0;
    context.slot         = TEST_SLOT_BEGIN;
    context.start_symbol = 0;
    context.format       = preamble_format::FORMAT0;
    context.rb_offset    = 0;
    context.pusch_scs    = subcarrier_spacing::kHz15;

    prach_buffer_spy buffer;

    // Do request.
    processor->get_request_handler().handle_request(buffer, context);

    // Check the expected notification.
    ASSERT_EQ(1, notifier_spy.get_nof_notifications());
    ASSERT_EQ(1, notifier_spy.get_request_overflow_entries().size());
    const auto& entry = notifier_spy.get_request_overflow_entries().back();
    ASSERT_EQ(context, entry);
    notifier_spy.clear_notifications();
  }

  // Verify no other entity was used.
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// Case of a PRACH request that full-filled with a single symbol.
TEST_P(prach_processor_test, single_baseband_symbol)
{
  // Prepare context with the count as sector.
  prach_buffer_context context;
  context.sector       = 1;
  context.port         = 3;
  context.slot         = TEST_SLOT_BEGIN;
  context.start_symbol = 1;
  context.format       = preamble_format::FORMAT0;
  context.rb_offset    = 123;
  context.pusch_scs    = subcarrier_spacing::kHz30;

  unsigned                   dft_size_15kHz   = std::get<0>(GetParam());
  unsigned                   sampling_rate_Hz = dft_size_15kHz * 15000;
  prach_preamble_information preamble_info    = get_prach_preamble_long_info(context.format);
  unsigned prach_window_length = (preamble_info.cp_length + preamble_info.symbol_length).to_samples(sampling_rate_Hz);
  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Verify no external entries are registered.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // Generate more samples than needed.
  std::vector<cf_t> samples(prach_window_length * 2);
  std::iota(samples.begin(), samples.end(), 0);

  // Prepare symbol context.
  prach_processor_baseband::symbol_context symbol_context;
  symbol_context.slot   = context.slot;
  symbol_context.symbol = context.start_symbol;
  symbol_context.sector = context.sector;
  symbol_context.port   = context.port;

  // Process samples.
  processor->get_baseband().process_symbol(samples, symbol_context);

  // Verify that the task is available for processing.
  ASSERT_TRUE(task_executor_spy.has_pending_tasks());

  // No demodulation neither notification entries.
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Run enqueued task.
  ASSERT_TRUE(task_executor_spy.try_run_next());

  // Verify the OFDM PRACH demodulator.
  ASSERT_EQ(1, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());

  // Verify demodulation entry.
  const auto demodulate_entry = ofdm_prach_factory_spy->get_total_demodulate_entries().back();
  ASSERT_EQ(demodulate_entry.buffer, &buffer);
  ASSERT_EQ(demodulate_entry.input.size(), prach_window_length);
  ASSERT_EQ(demodulate_entry.config.format, context.format);
  ASSERT_EQ(demodulate_entry.config.pusch_scs, context.pusch_scs);
  ASSERT_EQ(demodulate_entry.config.rb_offset, context.rb_offset);
  ASSERT_TRUE(std::equal(demodulate_entry.input.begin(), demodulate_entry.input.end(), samples.begin()));

  // Verify the received PRACH window has been processed.
  ASSERT_EQ(1, notifier_spy.get_nof_notifications());
  ASSERT_EQ(1, notifier_spy.get_rx_prach_window_entries().size());

  // Verify contents of the notification.
  const auto& rx_prach = notifier_spy.get_rx_prach_window_entries().back();
  ASSERT_EQ(rx_prach.context, context);
  ASSERT_EQ(rx_prach.buffer, &buffer);

  // No more tasks.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // No buffer method should have been called at any time.
  ASSERT_EQ(0, buffer.get_total_count());
}

// Case of a PRACH request that full-filled with three symbols.
TEST_P(prach_processor_test, three_baseband_symbol)
{
  // Prepare context with the count as sector.
  prach_buffer_context context;
  context.sector       = 1;
  context.port         = 3;
  context.slot         = TEST_SLOT_BEGIN;
  context.start_symbol = 1;
  context.format       = preamble_format::FORMAT0;
  context.rb_offset    = 123;
  context.pusch_scs    = subcarrier_spacing::kHz30;

  unsigned                   dft_size_15kHz   = std::get<0>(GetParam());
  unsigned                   sampling_rate_Hz = dft_size_15kHz * 15000;
  prach_preamble_information preamble_info    = get_prach_preamble_long_info(context.format);
  unsigned prach_window_length = (preamble_info.cp_length + preamble_info.symbol_length).to_samples(sampling_rate_Hz);
  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Verify no external entries are registered.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // Generate more samples than needed.
  std::vector<cf_t> samples(prach_window_length);
  std::iota(samples.begin(), samples.end(), 0);

  // Create three different views of samples.
  span<const cf_t> samples0 = span<cf_t>(samples).first(prach_window_length / 3);
  span<const cf_t> samples1 = span<cf_t>(samples).subspan(prach_window_length / 3, prach_window_length / 3);
  span<const cf_t> samples2 = span<cf_t>(samples).subspan((2 * prach_window_length) / 3, prach_window_length / 3);

  // Prepare symbol context.
  prach_processor_baseband::symbol_context symbol_context;
  symbol_context.slot   = context.slot;
  symbol_context.symbol = context.start_symbol - 1;
  symbol_context.sector = context.sector;
  symbol_context.port   = context.port;

  // Process samples - One symbol before the window starts.
  processor->get_baseband().process_symbol(samples, symbol_context);
  ++symbol_context.symbol;

  // No task, demodulation neither notification entries.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Process samples - Start of PRACH window.
  processor->get_baseband().process_symbol(samples0, symbol_context);
  ++symbol_context.symbol;

  // No task, demodulation neither notification entries.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  prach_processor_baseband::symbol_context rogue_symbol_context = symbol_context;
  ++rogue_symbol_context.sector;

  // Process samples from other sector. These should be ignored.
  processor->get_baseband().process_symbol(samples0, rogue_symbol_context);

  // No task, demodulation neither notification entries.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Process samples - Second segment.
  processor->get_baseband().process_symbol(samples1, symbol_context);
  ++symbol_context.symbol;

  // No task, demodulation neither notification entries.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Process samples - Third segment.
  processor->get_baseband().process_symbol(samples2, symbol_context);
  ++symbol_context.symbol;

  // Verify that the task is available for processing.
  ASSERT_TRUE(task_executor_spy.has_pending_tasks());

  // No demodulation neither notification entries.
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Process samples - Extra segment.
  processor->get_baseband().process_symbol(samples2, symbol_context);
  ++symbol_context.symbol;

  // Verify that the task is still available for processing.
  ASSERT_TRUE(task_executor_spy.has_pending_tasks());

  // No demodulation neither notification entries.
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());

  // Run enqueued task.
  ASSERT_TRUE(task_executor_spy.try_run_next());

  // Verify the OFDM PRACH demodulator.
  ASSERT_EQ(1, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());

  // Verify demodulation entry.
  const auto demodulate_entry = ofdm_prach_factory_spy->get_total_demodulate_entries().back();
  ASSERT_EQ(demodulate_entry.buffer, &buffer);
  ASSERT_EQ(demodulate_entry.input.size(), prach_window_length);
  ASSERT_EQ(demodulate_entry.config.format, context.format);
  ASSERT_EQ(demodulate_entry.config.pusch_scs, context.pusch_scs);
  ASSERT_EQ(demodulate_entry.config.rb_offset, context.rb_offset);
  ASSERT_TRUE(std::equal(demodulate_entry.input.begin(), demodulate_entry.input.end(), samples.begin()));

  // Verify the received PRACH window has been processed.
  ASSERT_EQ(1, notifier_spy.get_nof_notifications());
  ASSERT_EQ(1, notifier_spy.get_rx_prach_window_entries().size());

  // Verify contents of the notification.
  const auto& rx_prach = notifier_spy.get_rx_prach_window_entries().back();
  ASSERT_EQ(rx_prach.context, context);
  ASSERT_EQ(rx_prach.buffer, &buffer);

  // No more tasks.
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // No buffer method should have been called at any time.
  ASSERT_EQ(0, buffer.get_total_count());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(prach_processor,
                         prach_processor_test,
                         ::testing::Combine(::testing::Values(512, 1024, 2048), ::testing::Values(1, 4)));
