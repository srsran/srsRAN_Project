
#include "lower_phy_impl.h"

using namespace srsgnb;

radio_timestamp lower_phy_impl::process_ul_symbol(unsigned symbol_id)
{
  // Get receiver instance.
  radio_data_plane_receiver& receiver = radio.get_receiver();

  // Calculate symbol size. Assumes all sectors have the same symbol size.
  unsigned symbol_sz = modulators[0]->get_symbol_size(symbol_id);

  // For each stream, receive baseband signal.
  for (unsigned stream_id = 0; stream_id != radio_buffers.size(); ++stream_id) {
    // Prepare buffer size.
    radio_buffers[stream_id].resize(symbol_sz);

    // Receive baseband signal.
    receive_metadata[stream_id] = receiver.receive(radio_buffers[stream_id], stream_id);
  }

  // Align receive streams if misaligned.
  radio_timestamp aligned_receive_ts = receive_metadata[0].ts;
  // ...

  // Select resource grid from the UL pool.
  resource_grid_context ul_rg_context = {};
  ul_rg_context.sector                = 0;
  ul_rg_context.slot                  = ul_slot_context;
  resource_grid& ul_rg                = ul_rg_pool.get_resource_grid(ul_rg_context);

  // Demodulate signal.
  // ...
  ul_rg.set_all_zero();

  // Notify the received symbols.
  lower_phy_rx_symbol_context_t ul_symbol_context = {};
  ul_symbol_context.sector                        = 0;
  ul_symbol_context.slot                          = ul_slot_context;
  ul_symbol_context.nof_symbols                   = symbol_id;
  symbol_handler.notify_rx_symbol(ul_symbol_context, ul_rg);

  return aligned_receive_ts;
}

void lower_phy_impl::process_dl_symbol(unsigned symbol_id, radio_timestamp timestamp)
{
  // Prepare transmit metadata.
  radio_data_plane_transmitter::metadata transmit_metadata = {};
  transmit_metadata.ts                                     = timestamp + rx_to_tx_delay;

  // Get transmit resource grid buffer for the given slot.
  lower_phy_dl_rg_buffer& dl_grid_buffer = dl_rg_buffers[dl_slot_context.system_slot() % dl_rg_buffers.size()];

  // Resize radio buffers, assume all sectors and ports symbol sizes are the same.
  unsigned symbol_sz = modulators.front()->get_symbol_size(symbol_id);
  for (radio_baseband_buffer_dynamic& buffer : radio_buffers) {
    buffer.resize(symbol_sz);
  }

  // Iterate for each sector...
  for (unsigned sector_id = 0; sector_id != sectors.size(); ++sector_id) {
    // Select sector configuration.
    const lower_phy_sector_description& sector = sectors[sector_id];

    // Select transmit resource grid.
    const resource_grid_reader* dl_rg = dl_grid_buffer.get_grid(sector_id);

    // If there is no data available for the sector.
    if (dl_rg == nullptr) {
      // Log warning indicating the sector.
      logger.warning("Unavailable data to transmit for sector {} and symbol {}.", sector_id, symbol_id);
    }

    // Iterate for each port in the sector...
    for (unsigned port_id = 0; port_id != sector.port_mapping.size(); ++port_id) {
      // Select port mapping.
      const lower_phy_sector_port_mapping& port_mapping = sector.port_mapping[port_id];

      // Get buffer for the given port mapping.
      span<radio_sample_type> buffer =
          radio_buffers[port_mapping.stream_id].get_channel_buffer(port_mapping.channel_id);

      // Modulate symbol if the resource grid is available. Otherwise, set all to zeros.
      if (dl_rg != nullptr) {
        modulators[sector_id]->modulate(buffer, *dl_rg, port_id, symbol_id);
      } else {
        srsvec::zero(buffer);
      }
    }
  }

  // Transmit signal.
  radio_data_plane_transmitter& transmitter = radio.get_transmitter();
  for (unsigned stream_id = 0; stream_id != radio_buffers.size(); ++stream_id) {
    transmitter.transmit(stream_id, transmit_metadata, radio_buffers[stream_id]);
  }
}

void lower_phy_impl::process_slot()
{
  // Update logger context.
  logger.set_context(dl_slot_context.system_slot());

  // Notify slot boundary.
  lower_phy_timing_context_t timing_context = {};
  timing_context.slot                       = dl_slot_context + max_processing_delay_slots;
  timing_handler.notify_tti_boundary(timing_context);

  // For each symbol in the slot. Skip symbol processing if stop mis signaled.
  for (unsigned symbol_slot_idx = 0; symbol_slot_idx < nof_symbols_per_slot && !quit; ++symbol_slot_idx) {
    // Calculate the symbol index within the subframe.
    unsigned ul_symbol_subframe_idx = ul_slot_context.subframe_slot_index() * nof_symbols_per_slot + symbol_slot_idx;

    // Notify UL half slot.
    if (symbol_slot_idx == nof_symbols_per_slot / 2) {
      lower_phy_timing_context_t ul_timing_context = {};
      timing_context.slot                          = dl_slot_context;
      timing_handler.notify_ul_half_slot_boundary(ul_timing_context);
    }

    // Process uplink symbol.
    radio_timestamp rx_timestamp = process_ul_symbol(ul_symbol_subframe_idx);

    // Calculate the symbol index within the subframe.
    unsigned dl_symbol_subframe_idx = dl_slot_context.subframe_slot_index() * nof_symbols_per_slot + symbol_slot_idx;

    // Process downlink symbol.
    process_dl_symbol(dl_symbol_subframe_idx, rx_timestamp);
  }

  // Immediate return if stop was signaled.
  if (quit) {
    return;
  }

  // Notify the end of an uplink full slot.
  lower_phy_timing_context_t ul_timing_context = {};
  timing_context.slot                          = ul_slot_context;
  timing_handler.notify_ul_full_slot_boundary(ul_timing_context);

  // Reset DL resource grid buffers.
  dl_rg_buffers[dl_slot_context.system_slot() % dl_rg_buffers.size()].reset();
}

void lower_phy_impl::run(task_executor& realtime_task_executor)
{
  // Run slot.
  process_slot();

  // Increment slot.
  dl_slot_context++;
  ul_slot_context++;

  // Feedbacks the execution until stop is signaled.
  if (!quit) {
    realtime_task_executor.defer([this, &realtime_task_executor]() { run(realtime_task_executor); });
  }
}

void lower_phy_impl::send(const resource_grid_context& context, const resource_grid_reader& grid)
{
  // Calculate slot index circular buffer.
  unsigned slot_idx = context.slot.system_slot() % dl_rg_buffers.size();

  logger.debug("Writing DL resource grid for sector {} and slot {} in index {}.",
               context.sector,
               context.slot.system_slot(),
               slot_idx);

  // Set grid. Concurrent protection is at resource grid buffer level.
  dl_rg_buffers[slot_idx].set_grid(grid, context.sector);
}

lower_phy_impl::lower_phy_impl(const lower_phy_configuration& config) :
  logger(srslog::fetch_basic_logger("Low-PHY")),
  radio(*config.radio),
  symbol_handler(*config.symbol_handler),
  timing_handler(*config.timing_handler),
  modulator_factory(*config.modulator_factory),
  ul_rg_pool(*config.ul_resource_grid_pool),
  rx_to_tx_delay(static_cast<unsigned>(config.rx_to_tx_delay * (config.dft_size_15kHz * 15e3))),
  max_processing_delay_slots(config.max_processing_delay_slots),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  sectors(config.sectors),
  ul_slot_context(config.numerology, 0),
  dl_slot_context(ul_slot_context + config.ul_to_dl_slot_offset)
{
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  // Assert parameters.
  srsran_always_assert(std::isnormal(config.rx_to_tx_delay), "Invalid Rx to Tx delay.");
  srsran_always_assert(config.ul_to_dl_slot_offset > 0, "The UL to DL slot offset must be greater than 0.");
  srsran_always_assert((ul_slot_context.nof_slots_per_system_frame() % config.nof_dl_rg_buffers == 0) &&
                           (config.nof_dl_rg_buffers > config.max_processing_delay_slots),
                       "The number of DL resource grids ({}) must be divisor of the number of slots per system frame "
                       "({}) and greater than the maximum processing delay in slots ({}).",
                       config.nof_dl_rg_buffers,
                       ul_slot_context.nof_slots_per_system_frame(),
                       config.max_processing_delay_slots);

  logger.info(
      "Initialised with rx_to_tx_delay={:.4f} us ({} samples), ul_to_dl_slot_offset={}, max_processing_delay_slots={}.",
      config.rx_to_tx_delay,
      rx_to_tx_delay,
      config.ul_to_dl_slot_offset,
      config.max_processing_delay_slots);

  // Make sure sub-modules are valid.
  srsran_always_assert(config.radio != nullptr, "Invalid radio pointer.");
  srsran_always_assert(config.symbol_handler != nullptr, "Invalid symbol handler pointer.");
  srsran_always_assert(config.timing_handler != nullptr, "Invalid timing handler pointer.");
  srsran_always_assert(config.modulator_factory != nullptr, "Invalid modulation factory pointer.");
  srsran_always_assert(config.ul_resource_grid_pool != nullptr, "Invalid uplink resource grid pool pointer.");

  // Create radio buffers and receive metadata.
  for (unsigned nof_channels : config.nof_channels_per_stream) {
    radio_buffers.emplace_back(nof_channels, 2 * config.dft_size_15kHz);
  }
  receive_metadata.resize(config.nof_channels_per_stream.size());

  // For each sector, create modulator.
  for (const lower_phy_sector_description& sector : config.sectors) {
    // Prepare sector modulator.
    ofdm_modulator_configuration configuration = {};
    configuration.numerology                   = config.numerology;
    configuration.bw_rb                        = sector.bandwidth_rb;
    configuration.dft_size                     = config.dft_size_15kHz / pow2(config.numerology);
    configuration.cp                           = config.cp;
    configuration.scale                        = config.tx_scale;

    // Create modulator.
    modulators.emplace_back(modulator_factory.create_ofdm_symbol_modulator(configuration));

    // Make sure the modulator creation is successful.
    srsran_always_assert(modulators.back() != nullptr, "Error: failed to create OFDM modulator.");
  }

  // Create pool of transmit resource grids.
  for (unsigned slot_count = 0; slot_count != config.nof_dl_rg_buffers; ++slot_count) {
    dl_rg_buffers.emplace_back(lower_phy_dl_rg_buffer(config.sectors.size()));
  }
}

void lower_phy_impl::start(task_executor& realtime_task_exeutor)
{
  logger.info("Starting...");

  // Enqueue initial task.
  realtime_task_exeutor.execute([this, &realtime_task_exeutor]() {
    // Notify the initial slot boundaries.
    for (unsigned slot_count = 0; slot_count != max_processing_delay_slots; ++slot_count) {
      lower_phy_timing_context_t timing_context = {};
      timing_context.slot                       = dl_slot_context + slot_count;
      timing_handler.notify_tti_boundary(timing_context);
    }

    // Process first slot.
    run(realtime_task_exeutor);
  });
}

void lower_phy_impl::stop()
{
  logger.info("Stopping...");
  quit = true;
}
