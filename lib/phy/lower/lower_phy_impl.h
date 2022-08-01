/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lower_phy_state_fsm.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/gateways/baseband/baseband_gateway.h"
#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/phy/lower/lower_phy_controller.h"
#include "srsgnb/phy/lower/lower_phy_error_notifier.h"
#include "srsgnb/phy/lower/lower_phy_request_handler.h"
#include "srsgnb/phy/lower/lower_phy_rg_handler.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/phy/resource_grid_pool.h"

namespace srsgnb {

/// Describes the factory configuration.
struct lower_phy_common_configuration {
  /// Provides OFDM modulators. Each entry belongs to a different sector.
  std::vector<std::unique_ptr<ofdm_symbol_modulator>> modulators;
  /// Provides OFDM demodulators. Each entry belongs to a different sector.
  std::vector<std::unique_ptr<ofdm_symbol_demodulator>> demodulators;
};

template <class RG>
class lower_phy_rg_buffer
{
private:
  /// Stores resource grid pointers for every sector.
  std::vector<RG*> grids;
  /// Protects grids access from concurrent read/write.
  std::mutex mutex;

public:
  void set_nof_sectors(unsigned nof_sectors) { grids.resize(nof_sectors); }

  void set_grid(RG& grid, unsigned sector_id)
  {
    std::unique_lock<std::mutex> lock(mutex);
    grids[sector_id] = &grid;
  }

  RG* get_grid(unsigned sector_id)
  {
    std::unique_lock<std::mutex> lock(mutex);
    return grids[sector_id];
  }

  /// Resets all pointers.
  void reset()
  {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& rg : grids) {
      rg = nullptr;
    }
  }
};

/// Lower physical layer implementation.
// TODO: Reduce the inheritance list of this class by splitting it into smaller subcomponents that handle more specific
// responsabilities.
class lower_phy_impl : public lower_phy,
                       public lower_phy_controller,
                       public lower_phy_rg_handler,
                       public lower_phy_request_handler
{
private:
  /// Number of resource grid buffers.
  static constexpr unsigned NOF_RG_BUFFER = 40;

  /// Logger.
  srslog::basic_logger& logger;
  /// Baseband gateway transmitter.
  baseband_gateway_transmitter& transmitter;
  /// Baseband gateway receiver.
  baseband_gateway_receiver& receiver;
  /// Receive symbol handler.
  lower_phy_rx_symbol_notifier& rx_symbol_notifier;
  /// Timing boundary handler.
  lower_phy_timing_notifier& timing_notifier;
  /// Error handler.
  lower_phy_error_notifier& error_notifier;
  /// Container for downlink resource grids buffers. Each entry belongs to a slot.
  circular_array<lower_phy_rg_buffer<const resource_grid_reader>, NOF_RG_BUFFER> dl_rg_buffers;
  /// Container for uplink resource grids buffers. Each entry belongs to a slot.
  circular_array<lower_phy_rg_buffer<resource_grid>, NOF_RG_BUFFER> ul_rg_buffers;
  /// Container for radio baseband buffers for each stream. Common for transmit and receive. The number of entries
  /// indicates the number of streams.
  std::vector<baseband_gateway_buffer_dynamic> radio_buffers;
  /// Container for radio receive metadata for each stream. The number of entries indicates the number of streams.
  std::vector<baseband_gateway_receiver::metadata> receive_metadata;
  /// Container for OFDM modulators. Each entry belongs to a different sector.
  std::vector<std::unique_ptr<ofdm_symbol_modulator>> modulators;
  /// Container for OFDM demodulators. Each entry belongs to a different sector.
  std::vector<std::unique_ptr<ofdm_symbol_demodulator>> demodulators;
  /// Receive-to-transmit delay in clock ticks.
  const baseband_gateway_timestamp rx_to_tx_delay;
  /// Maximum allowed processing delay in slots.
  const unsigned max_processing_delay_slots;
  /// Number of symbols per slot.
  const unsigned nof_symbols_per_slot;
  /// Sector configurations.
  const std::vector<lower_phy_sector_description> sectors;
  /// Current uplink processing slot context.
  slot_point ul_slot_context = {};
  /// Current downlink processing slot context.
  slot_point dl_slot_context = {};
  /// Current symbol index within the processing slot.
  unsigned symbol_slot_idx = 0;
  /// State of the lower PHY finite-state machine.
  lower_phy_state_fsm state_fsm;

  /// \brief Processes an uplink symbol.
  /// \param[in] symbol_idx Symbol index within a subframe.
  /// \return The radio timestamp of the received block.
  baseband_gateway_timestamp process_ul_symbol(unsigned symbol_idx);

  /// \brief Processes a downlink symbol.
  /// \param[in] symbol_idx Symbol index within a subframe.
  /// \param[in] timestamp Radio timestamp for the transmitted symbol.
  void process_dl_symbol(unsigned symbol_idx, baseband_gateway_timestamp timestamp);

  /// \brief Processes uplink and downlink slots.
  void process_symbol();

  /// Runs the lower physical layer.
  void realtime_process_loop(task_executor& realtime_task_executor);

public:
  /// \brief Constructs a generic lower physical layer.
  /// \param[in] common_config Provides the factory specific necessary parameters to construct the lower physical
  /// layer.
  /// \param[in] config Provides the common lower PHY parameters to construct the lower physical layer.
  explicit lower_phy_impl(lower_phy_common_configuration& common_config, const lower_phy_configuration& config);

  // See interface for documentation.
  void start(task_executor& realtime_task_executor) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  void request_prach_window(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void request_uplink_slot(const resource_grid_context& context, resource_grid& grid) override;

  // See interface for documentation.
  void handle_resource_grid(const resource_grid_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  lower_phy_request_handler& get_request_handler() override { return *this; }

  // See interface for documentation.
  lower_phy_rg_handler& get_rg_handler() override { return *this; }

  // See interface for documentation.
  lower_phy_controller& get_controller() override { return *this; }
};

} // namespace srsgnb
