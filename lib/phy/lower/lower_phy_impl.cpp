/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_impl.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_baseband.h"
#include "srsgnb/phy/lower/processors/prach/prach_processor_request_handler.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

namespace {

class resource_grid_reader_empty : public resource_grid_reader
{
public:
  bool is_empty(unsigned /**/) const override { return true; }
  void get(span<cf_t> symbols, unsigned /**/, span<const resource_grid_coordinate> /**/) const override
  {
    srsvec::zero(symbols);
  }
  span<cf_t> get(span<cf_t> symbols, unsigned /**/, unsigned /**/, unsigned /**/, span<const bool> /**/) const override
  {
    srsvec::zero(symbols);
    return {};
  }

  span<cf_t> get(span<cf_t> symbols,
                 unsigned /**/,
                 unsigned /**/,
                 unsigned /**/,
                 const bounded_bitset<MAX_RB * NRE>& /**/) const override
  {
    srsvec::zero(symbols);
    return {};
  }
  void get(span<cf_t> symbols, unsigned /**/, unsigned /**/, unsigned /**/) const override { srsvec::zero(symbols); }
};

} // namespace

static const resource_grid_reader_empty rg_reader_empty;

baseband_gateway_timestamp lower_phy_impl::process_ul_symbol(unsigned symbol_id)
{
  // Get the transmission resource grid buffer for the given slot.
  lower_phy_rg_buffer<resource_grid>& ul_grid_buffer = ul_rg_buffers[ul_slot_context.system_slot()];

  // Calculate symbol size. Assumes all sectors have the same symbol size.
  unsigned symbol_sz = modulators[0]->get_symbol_size(symbol_id);

  // For each stream, receive the baseband signal.
  for (unsigned stream_id = 0, nof_streams = radio_buffers.size(); stream_id != nof_streams; ++stream_id) {
    // Prepare buffer size.
    radio_buffers[stream_id].resize(symbol_sz);

    // Receive the baseband signal.
    receive_metadata[stream_id] = receiver.receive(radio_buffers[stream_id], stream_id);
  }

  // Align receive streams if misaligned.
  baseband_gateway_timestamp aligned_receive_ts = receive_metadata[0].ts;
  // ...

  // Demodulate signal for each sector.
  for (unsigned sector_id = 0, nof_sectors = sectors.size(); sector_id != nof_sectors; ++sector_id) {
    // Select sector configuration.
    const lower_phy_sector_description& sector = sectors[sector_id];

    // Select resource grid from the UL pool and skip sector processing if there is no grid.
    resource_grid* ul_rg = ul_grid_buffer.get_grid(sector_id);

    // For each port of the sector.
    for (unsigned port_id = 0, nof_ports = sector.port_mapping.size(); port_id != nof_ports; ++port_id) {
      // Select port mapping.
      const lower_phy_sector_port_mapping& port_mapping = sector.port_mapping[port_id];

      // Get buffer for the given port mapping.
      span<const radio_sample_type> buffer =
          radio_buffers[port_mapping.stream_id].get_channel_buffer(port_mapping.channel_id);

      // Call PRACH processor.
      prach_processor_baseband::symbol_context prach_context;
      prach_context.slot   = ul_slot_context;
      prach_context.symbol = symbol_id;
      prach_context.sector = sector_id;
      prach_context.port   = port_id;
      prach_proc->get_baseband().process_symbol(buffer, prach_context);

      // Demodulate the baseband signal if it has been requested.
      if (ul_rg) {
        demodulators[sector_id]->demodulate(*ul_rg, buffer, port_id, symbol_id);
      }
    }

    // Skip the symbol notification when it has not been requested.
    if (!ul_rg) {
      continue;
    }

    // Notify the received symbols.
    lower_phy_rx_symbol_context ul_symbol_context = {};
    ul_symbol_context.sector                      = sector_id;
    ul_symbol_context.slot                        = ul_slot_context;
    ul_symbol_context.nof_symbols                 = symbol_id % nof_symbols_per_slot;
    rx_symbol_notifier.on_rx_symbol(ul_symbol_context, *ul_rg);
  }

  return aligned_receive_ts;
}

void lower_phy_impl::process_dl_symbol(unsigned symbol_id, baseband_gateway_timestamp timestamp)
{
  // Prepare transmit metadata.
  baseband_gateway_transmitter::metadata transmit_metadata = {};
  transmit_metadata.ts                                     = timestamp + rx_to_tx_delay;

  // Get transmit resource grid buffer for the given slot.
  lower_phy_rg_buffer<const resource_grid_reader>& dl_grid_buffer = dl_rg_buffers[dl_slot_context.system_slot()];

  // Resize radio buffers, assume all sectors and ports symbol sizes are the same.
  unsigned symbol_sz = modulators.front()->get_symbol_size(symbol_id);
  for (auto& buffer : radio_buffers) {
    buffer.resize(symbol_sz);
  }

  // Iterate for each sector...
  for (unsigned sector_id = 0, nof_sectors = sectors.size(); sector_id != nof_sectors; ++sector_id) {
    // Select sector configuration.
    const lower_phy_sector_description& sector = sectors[sector_id];

    // Select transmit resource grid.
    const resource_grid_reader* dl_rg = dl_grid_buffer.get_grid(sector_id);

    // If there is no data available for the sector.
    if (dl_rg == nullptr) {
      // Log warning indicating the sector.
      lower_phy_error_notifier::late_resource_grid_context context;
      context.sector = sector_id;
      context.slot   = dl_slot_context;
      context.symbol = symbol_id;
      error_notifier.on_late_resource_grid(context);
    }

    // Iterate for each port in the sector...
    for (unsigned port_id = 0, nof_ports = sector.port_mapping.size(); port_id != nof_ports; ++port_id) {
      // Select port mapping.
      const lower_phy_sector_port_mapping& port_mapping = sector.port_mapping[port_id];

      // Get buffer for the given port mapping.
      span<radio_sample_type> buffer =
          radio_buffers[port_mapping.stream_id].get_channel_buffer(port_mapping.channel_id);

      // Modulate symbol if the resource grid is available. Otherwise, set all to zeros.
      if (dl_rg == nullptr) {
        srsvec::zero(buffer);
        continue;
      }
      modulators[sector_id]->modulate(buffer, *dl_rg, port_id, symbol_id);

      // Process time domain signal with the amplitude controller.
      amplitude_controller_metrics amplitude_control_metrics =
          amplitude_controllers[sector_id]->process(buffer, buffer);

      if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - amplitude_log_tp) >=
          amplitude_log_period_ms) {
        // Log the amplitude controller metrics periodically.
        logger.debug(
            "Amplitude controller metrics: Mean pwr={:.2f} dBFS, Peak pwr={:.2f} dBFS, PAPR={:.2f} dB, Gain={:.2f} dB, "
            "Clipped sps={}, Clipping prob={:.2e}.",
            amplitude_control_metrics.avg_power_dBFS,
            amplitude_control_metrics.peak_power_dBFS,
            amplitude_control_metrics.papr_dB,
            amplitude_control_metrics.gain_dB,
            amplitude_control_metrics.nof_clipped_samples,
            amplitude_control_metrics.clipping_probability);

        // Update amplitude logging time point.
        amplitude_log_tp = std::chrono::steady_clock::now();
      }
    }
  }

  // Transmit signal.
  for (unsigned stream_id = 0, nof_streams = radio_buffers.size(); stream_id != nof_streams; ++stream_id) {
    transmitter.transmit(stream_id, transmit_metadata, radio_buffers[stream_id]);
  }
}

void lower_phy_impl::process_symbol()
{
  // Detect slot boundary.
  if (symbol_slot_idx == 0) {
    // Update logger context.
    logger.set_context(dl_slot_context.sfn(), dl_slot_context.slot_index());

    // Notify slot boundary.
    lower_phy_timing_context timing_context = {};
    timing_context.slot                     = dl_slot_context + max_processing_delay_slots;
    timing_notifier.on_tti_boundary(timing_context);
  }

  // Calculate the symbol index within the subframe.
  unsigned ul_symbol_subframe_idx = ul_slot_context.subframe_slot_index() * nof_symbols_per_slot + symbol_slot_idx;

  // Notify UL signal demodulation.
  if (symbol_slot_idx == nof_symbols_per_slot / 2) {
    // Notify the end of an uplink half slot.
    lower_phy_timing_context ul_timing_context = {};
    ul_timing_context.slot                     = ul_slot_context;
    timing_notifier.on_ul_half_slot_boundary(ul_timing_context);
  } else if (symbol_slot_idx == nof_symbols_per_slot - 1) {
    // Notify the end of an uplink full slot.
    lower_phy_timing_context ul_timing_context = {};
    ul_timing_context.slot                     = ul_slot_context;
    timing_notifier.on_ul_full_slot_boundary(ul_timing_context);
  }

  // Process uplink symbol.
  baseband_gateway_timestamp rx_timestamp = process_ul_symbol(ul_symbol_subframe_idx);

  // Calculate the symbol index within the subframe.
  unsigned dl_symbol_subframe_idx = dl_slot_context.subframe_slot_index() * nof_symbols_per_slot + symbol_slot_idx;

  // Process downlink symbol.
  process_dl_symbol(dl_symbol_subframe_idx, rx_timestamp);

  // Increment symbol index within the slot.
  ++symbol_slot_idx;

  // Detect symbol index overflow.
  if (symbol_slot_idx == nof_symbols_per_slot) {
    // Reset DL resource grid buffers.
    logger.debug("Clearing DL resource grid slot {}.", dl_slot_context.system_slot());
    dl_rg_buffers[dl_slot_context.system_slot()].reset();
    ul_rg_buffers[ul_slot_context.system_slot()].reset();

    // Reset the symbol index.
    symbol_slot_idx = 0;

    // Increment slot.
    dl_slot_context++;
    ul_slot_context++;
  }
}

void lower_phy_impl::realtime_process_loop(task_executor& realtime_task_executor)
{
  // Process symbol.
  process_symbol();

  // Feedbacks the task if no stop has been signaled.
  if (state_fsm.is_running()) {
    realtime_task_executor.defer([this, &realtime_task_executor]() { realtime_process_loop(realtime_task_executor); });
    return;
  }

  // Notify the stop of the asynchronous operation.
  state_fsm.on_async_executor_stop();
  logger.debug("Realtime process finished.");
}

void lower_phy_impl::handle_resource_grid(const resource_grid_context& context, const resource_grid_reader& grid)
{
  logger.debug("Writing DL resource grid for sector {} and slot {}.", context.sector, context.slot.system_slot());

  // Set grid. Concurrent protection is at resource grid buffer level.
  dl_rg_buffers[context.slot.system_slot()].set_grid(grid, context.sector);
}

lower_phy_impl::lower_phy_impl(lower_phy_common_configuration&& common_config, const lower_phy_configuration& config) :
  logger(srslog::fetch_basic_logger("Low-PHY")),
  transmitter(config.bb_gateway->get_transmitter()),
  receiver(config.bb_gateway->get_receiver()),
  rx_symbol_notifier(*config.rx_symbol_notifier),
  timing_notifier(*config.timing_notifier),
  error_notifier(*config.error_notifier),
  modulators(std::move(common_config.modulators)),
  demodulators(std::move(common_config.demodulators)),
  amplitude_controllers(std::move(common_config.amplitude_controllers)),
  prach_proc(std::move(common_config.prach_proc)),
  rx_to_tx_delay(get_rx_to_tx_delay(config.ul_to_dl_subframe_offset,
                                    config.time_advance_calibration,
                                    config.ta_offset,
                                    config.srate)),
  max_processing_delay_slots(config.max_processing_delay_slots),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  sectors(config.sectors),
  ul_slot_context(to_numerology_value(config.scs), 0),
  dl_slot_context(ul_slot_context + config.ul_to_dl_subframe_offset * ul_slot_context.nof_slots_per_subframe())
{
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  // Assert parameters.
  srsgnb_assert(config.ul_to_dl_subframe_offset > 0, "The UL to DL slot offset must be greater than 0.");

  logger.info("Initialized with time_advance_calibration={:.4f} us ({} samples), ul_to_dl_subframe_offset={}, "
              "max_processing_delay_slots={}.",
              config.time_advance_calibration.to_seconds() * 1e6,
              config.time_advance_calibration.to_samples(config.srate.to_Hz()),
              config.ul_to_dl_subframe_offset,
              config.max_processing_delay_slots);

  // Make sure dependencies are valid.
  srsgnb_assert(config.bb_gateway != nullptr, "Invalid baseband gateway pointer.");
  srsgnb_assert(config.error_notifier != nullptr, "Invalid error notifier.");
  srsgnb_assert(config.rx_symbol_notifier != nullptr, "Invalid symbol notifier pointer.");
  srsgnb_assert(config.timing_notifier != nullptr, "Invalid timing notifier pointer.");
  srsgnb_assert(modulators.size() == config.sectors.size(),
                "The number of sectors ({}) and modulators ({}) do not match.",
                config.sectors.size(),
                modulators.size());
  srsgnb_assert(demodulators.size() == config.sectors.size(),
                "The number of sectors ({}) and demodulators ({}) do not match.",
                config.sectors.size(),
                demodulators.size());
  srsgnb_assert(amplitude_controllers.size() == config.sectors.size(),
                "The number of sectors ({}) and amplitude controllers ({}) do not match.",
                config.sectors.size(),
                amplitude_controllers.size());
  for (auto& modulator : modulators) {
    srsgnb_assert(modulator, "Invalid modulator.");
  }
  for (auto& demodulator : demodulators) {
    srsgnb_assert(demodulator, "Invalid demodulator.");
  }
  for (auto& amplitude_controller : amplitude_controllers) {
    srsgnb_assert(amplitude_controller, "Invalid amplitude controller.");
  }

  // Create radio buffers and receive metadata.
  for (unsigned nof_channels : config.nof_channels_per_stream) {
    radio_buffers.emplace_back(nof_channels, 2 * config.srate.get_dft_size(15e3));
  }
  receive_metadata.resize(config.nof_channels_per_stream.size());

  // Create pool of transmit resource grids.
  unsigned ul_to_dl_slot_offset = config.ul_to_dl_subframe_offset * ul_slot_context.nof_slots_per_subframe();
  for (unsigned slot_count = 0, nof_slots = dl_rg_buffers.size(); slot_count != nof_slots; ++slot_count) {
    dl_rg_buffers[slot_count].set_nof_sectors(sectors.size());

    // If the slot is inside the start transition, then set an initial resource grid reader than is empty.
    if (slot_count >= ul_to_dl_slot_offset && slot_count < ul_to_dl_slot_offset + max_processing_delay_slots) {
      for (unsigned sector_id = 0, nof_sectors = sectors.size(); sector_id != nof_sectors; ++sector_id) {
        logger.debug("Writing initial DL resource grid for sector {} and slot {}.", sector_id, slot_count);
        dl_rg_buffers[slot_count].set_grid(rg_reader_empty, sector_id);
      }
    }
  }

  // Create pool of receive resource grids.
  for (auto& ul_rg_buffer : ul_rg_buffers) {
    ul_rg_buffer.set_nof_sectors(sectors.size());
  }

  // Signal a successful initialization.
  state_fsm.on_successful_init();

  // Connect notification adaptor to the notification interfaces.
  notification_adaptor.connect_error_notifier(*config.error_notifier);
  notification_adaptor.connect_rx_symbol_notifier(*config.rx_symbol_notifier);

  // Connect PRACH processor to the notification adaptor.
  prach_proc->connect(notification_adaptor.get_prach_notifier());
}

void lower_phy_impl::start(task_executor& realtime_task_executor)
{
  logger.info("Starting...");
  realtime_task_executor.defer([this, &realtime_task_executor]() { realtime_process_loop(realtime_task_executor); });
}

void lower_phy_impl::stop()
{
  logger.info("Stopping...");
  state_fsm.stop_and_join();
  logger.debug("Stopped successfully.");
}

void lower_phy_impl::request_prach_window(const prach_buffer_context& context, prach_buffer& buffer)
{
  prach_proc->get_request_handler().handle_request(buffer, context);
}

void lower_phy_impl::request_uplink_slot(const resource_grid_context& context, resource_grid& grid)
{
  logger.debug("Writing UL resource grid for sector {} and slot {}.", context.sector, context.slot.system_slot());
  ul_rg_buffers[context.slot.system_slot()].set_grid(grid, context.sector);
}
