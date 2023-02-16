/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../gateways/baseband/baseband_gateway_test_doubles.h"
#include "../../support/task_executor_test_doubles.h"
#include "lower_phy_test_doubles.h"
#include "modulation/ofdm_demodulator_test_doubles.h"
#include "modulation/ofdm_modulator_test_doubles.h"
#include "processors/prach/prach_processor_test_doubles.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

static sampling_rate       srate                      = sampling_rate::from_MHz(61.44);
static subcarrier_spacing  scs                        = subcarrier_spacing::kHz15;
static unsigned            max_processing_delay_slots = 4;
static unsigned            ul_to_dl_subframe_offset   = 1;
static int                 time_alignment_calibration = 0;
static lower_phy_ta_offset ta_offset                  = lower_phy_ta_offset::n0;
static float               tx_scale                   = 1.0F;
static cyclic_prefix       cp                         = cyclic_prefix::NORMAL;
static unsigned            bandwidth_rb               = 52;
static double              dl_freq_hz                 = 2.65e9;
static double              ul_freq_hz                 = 2.55e9;
static std::string         log_level                  = "debug";

static std::shared_ptr<ofdm_modulator_factory_spy>   modulator_factory         = nullptr;
static std::shared_ptr<ofdm_demodulator_factory_spy> demodulator_factory       = nullptr;
static std::shared_ptr<prach_processor_factory_spy>  prach_factory             = nullptr;
static std::shared_ptr<lower_phy_factory>            phy_factory               = nullptr;
static std::shared_ptr<amplitude_controller_factory> amplitude_control_factory = nullptr;

static lower_phy_configuration create_phy_config(baseband_gateway_spy&         bb_gateway,
                                                 lower_phy_error_notifier&     error_notifier,
                                                 lower_phy_rx_symbol_notifier& rx_symbol_notifier,
                                                 lower_phy_timing_notifier&    timing_notifier)
{
  lower_phy_configuration config;
  config.srate                      = srate;
  config.scs                        = scs;
  config.max_processing_delay_slots = max_processing_delay_slots;
  config.ul_to_dl_subframe_offset   = ul_to_dl_subframe_offset;
  config.time_alignment_calibration = time_alignment_calibration;
  config.ta_offset                  = ta_offset;
  config.tx_scale                   = tx_scale;
  config.cp                         = cp;
  config.dft_window_offset          = 0.0F;
  config.bb_gateway                 = &bb_gateway;
  config.rx_symbol_notifier         = &rx_symbol_notifier;
  config.timing_notifier            = &timing_notifier;
  config.error_notifier             = &error_notifier;
  config.prach_async_executor       = nullptr;
  config.amplitude_config           = {};
  config.sectors.emplace_back();
  config.sectors.back().bandwidth_rb = bandwidth_rb;
  config.sectors.back().dl_freq_hz   = dl_freq_hz;
  config.sectors.back().ul_freq_hz   = ul_freq_hz;
  config.sectors.back().port_mapping.emplace_back();
  config.sectors.back().port_mapping.back().stream_id  = 0;
  config.sectors.back().port_mapping.back().channel_id = 0;
  config.nof_channels_per_stream                       = {1};
  config.log_level                                     = log_level;

  return config;
}

static void test_start_run_stop()
{
  baseband_gateway_spy             bb_gateway(log_level);
  lower_phy_error_notifier_spy     error_notifier(log_level);
  lower_phy_rx_symbol_notifier_spy rx_symbol_notifier(log_level);
  lower_phy_timing_notifier_spy    timing_notifier(log_level);

  // Calculate Rx-to-Tx delay in samples.
  unsigned rx_to_tx_delay = phy_time_unit::from_seconds(0.001 * ul_to_dl_subframe_offset).to_samples(srate.to_Hz()) -
                            time_alignment_calibration +
                            phy_time_unit::from_units_of_Tc(static_cast<unsigned>(ta_offset)).to_samples(srate.to_Hz());

  lower_phy_configuration phy_config =
      create_phy_config(bb_gateway, error_notifier, rx_symbol_notifier, timing_notifier);

  std::unique_ptr<lower_phy> phy_instance = phy_factory->create(phy_config);
  TESTASSERT(phy_instance);
  lower_phy_controller& phy = phy_instance->get_controller();

  ofdm_symbol_modulator_spy*   modulator   = modulator_factory->get_modulators().back();
  ofdm_symbol_demodulator_spy* demodulator = demodulator_factory->get_demodulators().back();

  manual_task_worker_always_enqueue_tasks manual_task_executor(1);
  TESTASSERT(!manual_task_executor.has_pending_tasks());

  // Start PHY.
  phy.start(manual_task_executor);

  // Assert internal components.
  TESTASSERT(manual_task_executor.has_pending_tasks());
  TESTASSERT(modulator->get_modulate_entries().empty());
  TESTASSERT(demodulator->get_demodulate_entries().empty());

  // Assert for no baseband transmission nor reception.
  TESTASSERT(bb_gateway.get_receive_entries().empty());
  TESTASSERT(bb_gateway.get_transmit_entries().empty());

  // Assert for no notifications of any type.
  TESTASSERT_EQ(0, rx_symbol_notifier.get_nof_events());
  TESTASSERT_EQ(0, timing_notifier.get_nof_events());

  // Run the start up transition.
  for (unsigned slot_count = 0; slot_count != 10 * max_processing_delay_slots; ++slot_count) {
    // Derive downlink and uplink expected slots.
    slot_point expected_dl_slot(to_numerology_value(scs), slot_count + max_processing_delay_slots);
    slot_point expected_ul_slot(to_numerology_value(scs), slot_count);
    slot_point expected_late_slot(to_numerology_value(scs), slot_count + ul_to_dl_subframe_offset);

    // Run all symbols in the slot.
    for (unsigned symbol_count = 0; symbol_count != get_nsymb_per_slot(cp); ++symbol_count) {
      // Derive the expected symbol size including the cyclic prefix.
      unsigned expected_symbol_sz = srate.get_dft_size(scs);
      expected_symbol_sz += cp.get_length(symbol_count, scs).to_samples(srate.to_Hz());

      // Run a single symbol.
      TESTASSERT(manual_task_executor.try_run_next());

      // Verify baseband gateway calls.
      TESTASSERT_EQ(1, bb_gateway.get_receive_entries().size());
      const auto& bb_gateway_rx_entry = bb_gateway.get_receive_entries().front();
      TESTASSERT_EQ(0, bb_gateway_rx_entry.stream_id);
      TESTASSERT_EQ(expected_symbol_sz, bb_gateway_rx_entry.data.get_nof_samples());

      TESTASSERT_EQ(1, bb_gateway.get_transmit_entries().size());
      const auto& bb_gateway_tx_entry = bb_gateway.get_transmit_entries().front();
      TESTASSERT_EQ(0, bb_gateway_tx_entry.stream_id);
      TESTASSERT_EQ(expected_symbol_sz, bb_gateway_tx_entry.data.get_nof_samples());
      TESTASSERT_EQ(bb_gateway_rx_entry.metadata.ts + rx_to_tx_delay, bb_gateway_tx_entry.metadata.ts);

      // Detect slot boundary.
      if (symbol_count == 0) {
        TESTASSERT_EQ(1,
                      timing_notifier.get_tti_boundaries_events().size(),
                      "slot_count={}; symbol_count={};",
                      slot_count,
                      symbol_count);
        const auto& timing_notifier_entry = timing_notifier.get_tti_boundaries_events().front();
        TESTASSERT_EQ(expected_dl_slot, timing_notifier_entry.slot);
      } else {
        TESTASSERT(timing_notifier.get_tti_boundaries_events().empty(),
                   "slot_count={}; symbol_count={};",
                   slot_count,
                   symbol_count);
      }

      // Detect UL timing notifications.
      if (symbol_count == get_nsymb_per_slot(cp) / 2) {
        TESTASSERT_EQ(1, timing_notifier.get_ul_half_slot_events().size());
        const auto& timing_notifier_entry = timing_notifier.get_ul_half_slot_events().front();
        TESTASSERT_EQ(expected_ul_slot, timing_notifier_entry.slot);
      } else if (symbol_count == get_nsymb_per_slot(cp) - 1) {
        TESTASSERT_EQ(1, timing_notifier.get_ul_full_slot_events().size());
        const auto& timing_notifier_entry = timing_notifier.get_ul_full_slot_events().front();
        TESTASSERT_EQ(expected_ul_slot, timing_notifier_entry.slot);
      } else {
        TESTASSERT(timing_notifier.get_ul_half_slot_events().empty());
      }

      // Make sure next slot is queued in the executor.
      TESTASSERT(manual_task_executor.has_pending_tasks());

      // Depending on whether the lower PHY is in the start transition.
      if (slot_count < max_processing_delay_slots) {
        // Expect modulator entry.
        TESTASSERT_EQ(1, modulator->get_modulate_entries().size());
        const auto& modulate_entry = modulator->get_modulate_entries().front();
        TESTASSERT_EQ(0, modulate_entry.port_index);
        TESTASSERT_EQ(symbol_count, modulate_entry.symbol_index);
        TESTASSERT_EQ(true, modulate_entry.grid->is_empty(modulate_entry.port_index));
        TESTASSERT_EQ(0, modulate_entry.port_index);

        // Do not expect any late event.
        TESTASSERT_EQ(0, error_notifier.get_nof_errors());
      } else {
        // Do NOT expect any modulation entry as it was not send any grid.
        TESTASSERT(modulator->get_modulate_entries().empty());

        // Expect a RG late event.
        TESTASSERT_EQ(1, error_notifier.get_nof_errors());

        const auto& error_notifier_late_entry = error_notifier.get_late_rg_errors().front();
        TESTASSERT_EQ(0, error_notifier_late_entry.sector);
        TESTASSERT_EQ(expected_late_slot, error_notifier_late_entry.slot);
        TESTASSERT_EQ(symbol_count, error_notifier_late_entry.symbol);
      }

      // Verify the transmitted baseband matches the modulator output.
      if (!modulator->get_modulate_entries().empty()) {
        // Generate expected modulated baseband buffer.
        const auto&       modulate_entry   = modulator->get_modulate_entries().front();
        std::vector<cf_t> modulated_buffer = modulate_entry.output;
        srsvec::sc_prod(modulated_buffer, tx_scale, modulated_buffer);
        TESTASSERT_EQ(bb_gateway_tx_entry.data[0], span<const cf_t>(modulated_buffer));
      }

      // Do NOT expect any uplink demodulation as it was not requested.
      TESTASSERT(demodulator->get_demodulate_entries().empty());

      // Clear spies.
      modulator->clear_modulate_entries();
      bb_gateway.clear_all_entries();
      error_notifier.clear_all_errors();
      rx_symbol_notifier.clear_all_events();
      timing_notifier.clear_all_events();
    }
  }

  // Signal stop (and joint) asynchronously.
  std::atomic<bool> joined = {false};
  std::thread       async([&phy, &joined]() {
    phy.stop();
    joined = true;
  });
  while (!joined) {
    manual_task_executor.try_run_next();
  }
  async.join();

  // No pending task.
  TESTASSERT(!manual_task_executor.has_pending_tasks());
}

int main()
{
  srslog::init();

  modulator_factory = std::make_shared<ofdm_modulator_factory_spy>(log_level);
  TESTASSERT(modulator_factory);
  demodulator_factory = std::make_shared<ofdm_demodulator_factory_spy>();
  TESTASSERT(demodulator_factory);
  demodulator_factory = std::make_shared<ofdm_demodulator_factory_spy>();
  TESTASSERT(demodulator_factory);
  prach_factory = std::make_shared<prach_processor_factory_spy>();
  TESTASSERT(demodulator_factory);

  // Keep the amplitude control clipping disabled throughout the test.
  amplitude_controller_clipping_config amplitude_config = {false, 0.0F, 1.0F, 0.0F};
  amplitude_control_factory = create_amplitude_controller_clipping_factory(amplitude_config);
  TESTASSERT(amplitude_control_factory);

  phy_factory =
      create_lower_phy_factory_sw(modulator_factory, demodulator_factory, prach_factory, amplitude_control_factory);
  TESTASSERT(phy_factory);

  test_start_run_stop();
  return 0;
}
