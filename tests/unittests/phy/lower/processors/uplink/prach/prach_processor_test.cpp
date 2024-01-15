/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../../../../../support/task_executor_test_doubles.h"
#include "../../../../support/prach_buffer_test_doubles.h"
#include "../../../modulation/ofdm_prach_demodulator_test_doubles.h"
#include "prach_processor_notifier_test_doubles.h"
#include "prach_processor_test_doubles.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader_view.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <numeric>
#include <random>

namespace srsran {

std::ostream& operator<<(std::ostream& os, subcarrier_spacing value)
{
  fmt::print(os, "{}", to_string(value));
  return os;
}

std::ostream& operator<<(std::ostream& os, prach_format_type value)
{
  fmt::print(os, "Format {}", to_string(value));
  return os;
}

std::ostream& operator<<(std::ostream& os, sampling_rate value)
{
  fmt::print(os, "{}", value);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

auto to_tuple(const prach_buffer_context& context)
{
  return std::tie(context.sector,
                  context.ports,
                  context.slot,
                  context.start_symbol,
                  context.format,
                  context.rb_offset,
                  context.nof_td_occasions,
                  context.nof_fd_occasions,
                  context.nof_prb_ul_grid,
                  context.pusch_scs,
                  context.root_sequence_index,
                  context.restricted_set,
                  context.zero_correlation_zone,
                  context.start_preamble_index,
                  context.nof_preamble_indices);
}

bool operator==(const prach_buffer_context& rhs, const prach_buffer_context& lhs)
{
  return to_tuple(rhs) == to_tuple(lhs);
}

bool operator==(span<const cf_t> rhs, span<const cf_t> lhs)
{
  return std::equal(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

} // namespace srsran

using namespace srsran;

// Combined parameters. In order, PRACH format, PUSCH subcarrier spacing and sampling rate.
using PrachProcessorParams = std::tuple<prach_format_type, subcarrier_spacing, sampling_rate>;

static constexpr slot_point TEST_SLOT_BEGIN = slot_point(0, 0);
static constexpr slot_point TEST_SLOT_END   = slot_point(0, 10);

class PrachProcessorFixture : public ::testing::TestWithParam<PrachProcessorParams>
{
protected:
  PrachProcessorFixture() : task_executor_spy(1), notifier_spy("none")
  {
    // Do nothing.
  }

  void SetUp() override
  {
    Test::SetUp();

    // Get parameters.
    format    = std::get<0>(GetParam());
    pusch_scs = std::get<1>(GetParam());
    srate     = std::get<2>(GetParam());

    // Select a random number of maximum concurrent requests.
    max_nof_concurrent_requests = max_nof_concurrent_requests_dist(rgen);

    // Create OFDM PRACH demodulator spy.
    ofdm_prach_factory_spy = std::make_shared<ofdm_prach_demodulator_factory_spy>();

    // Create factory.
    std::shared_ptr<prach_processor_factory> factory = create_prach_processor_factory_sw(
        ofdm_prach_factory_spy, task_executor_spy, srate, max_nof_ports, max_nof_concurrent_requests);
    ASSERT_TRUE(factory);

    // Create processor.
    processor = factory->create();
    ASSERT_TRUE(processor);

    // Connect processor with the notifier.
    processor->connect(notifier_spy);

    // Validate that the number of OFDM PRACH demodulators match with the maximum number of concurrent requests.
    ASSERT_EQ(ofdm_prach_factory_spy->get_entries().size(), max_nof_concurrent_requests);

    // Clear notifications.
    notifier_spy.clear_notifications();
  }

  static constexpr unsigned unused_integer = UINT32_MAX;
  static constexpr unsigned max_nof_ports  = 1;

  std::shared_ptr<ofdm_prach_demodulator_factory_spy> ofdm_prach_factory_spy;
  std::unique_ptr<prach_processor>                    processor;
  manual_task_worker_always_enqueue_tasks             task_executor_spy;
  prach_processor_notifier_spy                        notifier_spy;

  prach_format_type  format;
  subcarrier_spacing pusch_scs;
  sampling_rate      srate;
  unsigned           max_nof_concurrent_requests;

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> start_symbol_index_dist;
  static std::uniform_int_distribution<unsigned> nof_td_occasions_dist;
  static std::uniform_int_distribution<unsigned> nof_fd_occasions_dist;
  static std::uniform_int_distribution<unsigned> max_nof_concurrent_requests_dist;
  static std::uniform_int_distribution<unsigned> nof_prb_ul_grid_dist;
  static std::uniform_int_distribution<unsigned> rb_offset_dist;
};

std::mt19937                            PrachProcessorFixture::rgen;
std::uniform_int_distribution<unsigned> PrachProcessorFixture::start_symbol_index_dist(0, 8);
std::uniform_int_distribution<unsigned> PrachProcessorFixture::nof_td_occasions_dist(1, 3);
std::uniform_int_distribution<unsigned> PrachProcessorFixture::nof_fd_occasions_dist(1, 8);
std::uniform_int_distribution<unsigned> PrachProcessorFixture::max_nof_concurrent_requests_dist(1, 16);
std::uniform_int_distribution<unsigned> PrachProcessorFixture::nof_prb_ul_grid_dist(25, 275);
std::uniform_int_distribution<unsigned> PrachProcessorFixture::rb_offset_dist(0, 270);

// If there is no request, the PRACH processor shall:
// - not notify any event,
// - not demodulate any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(PrachProcessorFixture, NoRequest)
{
  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = i_symbol;
      symbol_context.sector = 0;

      baseband_gateway_buffer_dynamic samples(0, 0);

      processor->get_baseband().process_symbol(samples.get_reader(), symbol_context);
    }
  }

  // Verify no entity was used.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());
}

// If the sector does not match with the PRACH request context, the PRACH processor shall:
// - not use the buffer methods,
// - not notify any event,
// - not process any OFDM PRACH window, and
// - not enqueue any task.
TEST_P(PrachProcessorFixture, SectorUnmatch)
{
  unsigned sector       = 0;
  unsigned rogue_sector = 1;

  prach_buffer_context context;
  context.sector                = rogue_sector;
  context.ports                 = {0};
  context.slot                  = TEST_SLOT_END;
  context.start_symbol          = start_symbol_index_dist(rgen);
  context.format                = format;
  context.rb_offset             = rb_offset_dist(rgen);
  context.nof_td_occasions      = nof_td_occasions_dist(rgen);
  context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
  context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
  context.pusch_scs             = pusch_scs;
  context.root_sequence_index   = unused_integer;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = unused_integer;
  context.start_preamble_index  = unused_integer;
  context.nof_preamble_indices  = unused_integer;

  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = i_symbol;
      symbol_context.sector = sector;

      baseband_gateway_buffer_dynamic samples(0, 0);

      processor->get_baseband().process_symbol(samples.get_reader(), symbol_context);
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
TEST_P(PrachProcessorFixture, DetectLate)
{
  prach_buffer_context context;
  context.sector                = 0;
  context.ports                 = {0};
  context.slot                  = TEST_SLOT_BEGIN;
  context.start_symbol          = start_symbol_index_dist(rgen);
  context.format                = format;
  context.rb_offset             = rb_offset_dist(rgen);
  context.nof_td_occasions      = nof_td_occasions_dist(rgen);
  context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
  context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
  context.pusch_scs             = pusch_scs;
  context.root_sequence_index   = unused_integer;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = unused_integer;
  context.start_preamble_index  = unused_integer;
  context.nof_preamble_indices  = unused_integer;

  // Do request.
  prach_buffer_spy buffer;
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = context.slot + 1; slot != TEST_SLOT_END; ++slot) {
    for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = i_symbol;
      symbol_context.sector = 0;

      baseband_gateway_buffer_dynamic samples(0, 0);

      processor->get_baseband().process_symbol(samples.get_reader(), symbol_context);

      if ((slot == context.slot + 1) && (i_symbol == 0)) {
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
TEST_P(PrachProcessorFixture, DetectSectorUnmatchLate)
{
  unsigned sector       = 0;
  unsigned rogue_sector = 1;

  prach_buffer_context context;
  context.sector                = rogue_sector;
  context.ports                 = {0};
  context.slot                  = TEST_SLOT_BEGIN;
  context.start_symbol          = start_symbol_index_dist(rgen);
  context.format                = format;
  context.rb_offset             = rb_offset_dist(rgen);
  context.nof_td_occasions      = nof_td_occasions_dist(rgen);
  context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
  context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
  context.pusch_scs             = pusch_scs;
  context.root_sequence_index   = unused_integer;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = unused_integer;
  context.start_preamble_index  = unused_integer;
  context.nof_preamble_indices  = unused_integer;

  prach_buffer_spy buffer;

  // Do request.
  processor->get_request_handler().handle_request(buffer, context);

  // Run a number of slots and symbols.
  for (slot_point slot = TEST_SLOT_BEGIN; slot != TEST_SLOT_END; ++slot) {
    for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      prach_processor_baseband::symbol_context symbol_context;
      symbol_context.slot   = slot;
      symbol_context.symbol = i_symbol;
      symbol_context.sector = sector;

      baseband_gateway_buffer_dynamic samples(0, 0);

      processor->get_baseband().process_symbol(samples.get_reader(), symbol_context);

      if (slot == context.slot + 1 && i_symbol == 0) {
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
TEST_P(PrachProcessorFixture, RequestOverflow)
{
  // For the maximum number of concurrent requests...
  for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
    // Prepare context with the count as sector.
    prach_buffer_context context;
    context.sector                = count;
    context.ports                 = {0};
    context.slot                  = TEST_SLOT_BEGIN;
    context.start_symbol          = start_symbol_index_dist(rgen);
    context.format                = format;
    context.rb_offset             = rb_offset_dist(rgen);
    context.nof_td_occasions      = nof_td_occasions_dist(rgen);
    context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
    context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
    context.pusch_scs             = pusch_scs;
    context.root_sequence_index   = unused_integer;
    context.restricted_set        = restricted_set_config::UNRESTRICTED;
    context.zero_correlation_zone = unused_integer;
    context.start_preamble_index  = unused_integer;
    context.nof_preamble_indices  = unused_integer;

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
    context.sector                = count + max_nof_concurrent_requests;
    context.ports                 = {0};
    context.slot                  = TEST_SLOT_BEGIN;
    context.start_symbol          = start_symbol_index_dist(rgen);
    context.format                = format;
    context.rb_offset             = rb_offset_dist(rgen);
    context.nof_td_occasions      = nof_td_occasions_dist(rgen);
    context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
    context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
    context.pusch_scs             = pusch_scs;
    context.root_sequence_index   = unused_integer;
    context.restricted_set        = restricted_set_config::UNRESTRICTED;
    context.zero_correlation_zone = unused_integer;
    context.start_preamble_index  = unused_integer;
    context.nof_preamble_indices  = unused_integer;

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

// The PRACH processor is requested to process one request. The processor is fed a single buffer of twice the size of
// the PRACH window.
TEST_P(PrachProcessorFixture, SingleBasebandSymbols)
{
  // Prepare context with the count as sector.
  prach_buffer_context context;
  context.sector                = 1;
  context.ports                 = {3};
  context.slot                  = TEST_SLOT_BEGIN;
  context.start_symbol          = start_symbol_index_dist(rgen);
  context.format                = format;
  context.rb_offset             = rb_offset_dist(rgen);
  context.nof_td_occasions      = nof_td_occasions_dist(rgen);
  context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
  context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
  context.pusch_scs             = pusch_scs;
  context.root_sequence_index   = unused_integer;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = unused_integer;
  context.start_preamble_index  = unused_integer;
  context.nof_preamble_indices  = unused_integer;

  // Calculate PRACH window size.
  unsigned prach_window_length =
      get_prach_window_duration(format, pusch_scs, context.start_symbol, context.nof_td_occasions)
          .to_samples(srate.to_Hz());

  // Do request.
  prach_buffer_spy buffer;
  processor->get_request_handler().handle_request(buffer, context);

  // Verify no external entries are registered.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // Generate more samples than needed.
  baseband_gateway_buffer_dynamic samples(context.ports.front() + 1, prach_window_length * 2);
  {
    span<cf_t> samples2 = samples[context.ports.front()];
    std::iota(samples2.begin(), samples2.end(), 0);
  }

  // Prepare symbol context.
  prach_processor_baseband::symbol_context symbol_context;
  symbol_context.slot   = context.slot;
  symbol_context.symbol = 0;
  symbol_context.sector = context.sector;

  // Process samples.
  processor->get_baseband().process_symbol(samples.get_reader(), symbol_context);

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
  ASSERT_EQ(demodulate_entry.config.rb_offset, context.rb_offset);
  ASSERT_EQ(demodulate_entry.config.nof_prb_ul_grid, context.nof_prb_ul_grid);
  ASSERT_EQ(demodulate_entry.config.pusch_scs, context.pusch_scs);
  ASSERT_EQ(span<const cf_t>(demodulate_entry.input),
            samples.get_reader().get_channel_buffer(context.ports.front()).first(prach_window_length));

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

// The PRACH processor is requested to process one request. The PRACH window is segmented in three different buffers.
TEST_P(PrachProcessorFixture, ThreeBasebandSymbols)
{
  // Prepare context with the count as sector.
  prach_buffer_context context;
  context.sector                = 1;
  context.ports                 = {3};
  context.slot                  = TEST_SLOT_BEGIN;
  context.start_symbol          = start_symbol_index_dist(rgen);
  context.format                = format;
  context.rb_offset             = rb_offset_dist(rgen);
  context.nof_td_occasions      = nof_td_occasions_dist(rgen);
  context.nof_fd_occasions      = nof_fd_occasions_dist(rgen);
  context.nof_prb_ul_grid       = nof_prb_ul_grid_dist(rgen);
  context.pusch_scs             = pusch_scs;
  context.root_sequence_index   = unused_integer;
  context.restricted_set        = restricted_set_config::UNRESTRICTED;
  context.zero_correlation_zone = unused_integer;
  context.start_preamble_index  = unused_integer;
  context.nof_preamble_indices  = unused_integer;

  // Calculate PRACH window size.
  unsigned prach_window_length =
      get_prach_window_duration(format, pusch_scs, context.start_symbol, context.nof_td_occasions)
          .to_samples(srate.to_Hz());

  // Do request.
  prach_buffer_spy buffer;
  processor->get_request_handler().handle_request(buffer, context);

  // Verify no external entries are registered.
  ASSERT_EQ(0, notifier_spy.get_nof_notifications());
  ASSERT_EQ(0, ofdm_prach_factory_spy->get_nof_total_demodulate_entries());
  ASSERT_FALSE(task_executor_spy.has_pending_tasks());

  // Generate more samples than needed.
  baseband_gateway_buffer_dynamic samples(context.ports.front() + 1, prach_window_length * 2);
  {
    span<cf_t> samples2 = samples[context.ports.front()];
    std::iota(samples2.begin(), samples2.end(), 0);
  }

  // Create three different views of samples.
  unsigned                            buffer_size = divide_ceil(prach_window_length, 3);
  baseband_gateway_buffer_reader_view samples0(samples.get_reader(), 0, buffer_size);
  baseband_gateway_buffer_reader_view samples1(samples.get_reader(), buffer_size, buffer_size);
  baseband_gateway_buffer_reader_view samples2(
      samples.get_reader(), 2 * buffer_size, samples.get_nof_samples() - 2 * buffer_size);

  // Prepare symbol context.
  prach_processor_baseband::symbol_context symbol_context;
  symbol_context.slot   = context.slot;
  symbol_context.symbol = 0;
  symbol_context.sector = context.sector;

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
  ASSERT_EQ(demodulate_entry.config.rb_offset, context.rb_offset);
  ASSERT_EQ(demodulate_entry.config.nof_prb_ul_grid, context.nof_prb_ul_grid);
  ASSERT_EQ(demodulate_entry.config.pusch_scs, context.pusch_scs);
  ASSERT_EQ(span<const cf_t>(demodulate_entry.input),
            samples.get_reader().get_channel_buffer(context.ports.front()).first(prach_window_length));

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

// Creates test suite that combines a few parameters.
INSTANTIATE_TEST_SUITE_P(prach_processor,
                         PrachProcessorFixture,
                         ::testing::Combine(::testing::Values(prach_format_type::zero,
                                                              prach_format_type::one,
                                                              prach_format_type::two,
                                                              prach_format_type::three,
                                                              prach_format_type::A1,
                                                              prach_format_type::A2,
                                                              prach_format_type::A3,
                                                              prach_format_type::B1,
                                                              prach_format_type::B4,
                                                              prach_format_type::C0,
                                                              prach_format_type::C2,
                                                              prach_format_type::A1_B1,
                                                              prach_format_type::A2_B2,
                                                              prach_format_type::A3_B3),
                                            ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30),
                                            ::testing::Values(sampling_rate::from_MHz(30.72))));
