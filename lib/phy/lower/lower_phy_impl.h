
#ifndef SRSGNB_LIB_PHY_LOWER_LOWER_PHY_IMPL_H
#define SRSGNB_LIB_PHY_LOWER_LOWER_PHY_IMPL_H

#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/phy/lower/lower_phy_control.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/phy/resource_grid_pool.h"
#include "srsgnb/radio/radio_data_plane.h"
#include "srsgnb/srsvec/zero.h"
#include <mutex>

namespace srsgnb {

class lower_phy_dl_rg_buffer
{
private:
  /// Stores resource grid pointers for every sector.
  std::vector<const resource_grid_reader*> grids;
  /// Protects grids access from concurrent read/write.
  std::mutex mutex;

public:
  /// Default constructor. Allow implicit construction.
  lower_phy_dl_rg_buffer(unsigned nof_sectors) : grids(nof_sectors) {}

  /// Move constructor.
  lower_phy_dl_rg_buffer(lower_phy_dl_rg_buffer&& other) : grids(std::move(other.grids)) {}

  void set_grid(const resource_grid_reader& grid, unsigned sector_id)
  {
    std::unique_lock<std::mutex> lock(mutex);
    grids[sector_id] = &grid;
  }

  const resource_grid_reader* get_grid(unsigned sector_id)
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

/// Describes a generic lower physical layer.
class lower_phy_impl : public lower_phy_control, private lower_phy_input_gateway
{
private:
  /// Logger.
  srslog::basic_logger& logger;
  /// Radio data plane.
  radio_data_plane& radio;
  /// Receive symbol handler.
  lower_phy_rx_symbol_handler& symbol_handler;
  /// Timing boundary handler.
  lower_phy_timing_handler& timing_handler;
  /// OFDM modulator factory.
  ofdm_modulator_factory& modulator_factory;
  /// Uplink resource grid pool.
  resource_grid_pool& ul_rg_pool;
  /// Stores resource grids buffers. Each entry belongs to a slot.
  std::vector<lower_phy_dl_rg_buffer> dl_rg_buffers;
  /// Stores radio baseband buffers for each stream. Common for transmit and receive. The number of entries indicates
  /// the number of streams.
  std::vector<radio_baseband_buffer_dynamic> radio_buffers;
  /// Stores radio receive metadata for each stream. The number of entries indicates the number of streams.
  std::vector<radio_data_plane_receiver::metadata> receive_metadata;
  /// Stores OFDM modulator. Each entry belongs to a different sector.
  std::vector<std::unique_ptr<ofdm_symbol_modulator> > modulators;
  /// Indicates the receive to transmit delay in clock ticks.
  const radio_timestamp rx_to_tx_delay;
  /// Indicates the maximum allowed processing delay in slots.
  const unsigned max_processing_delay_slots;
  /// Indicates the number of symbols per slot.
  const unsigned nof_symbols_per_slot;
  /// Provides the sectors configuration.
  const std::vector<lower_phy_sector_description> sectors;
  /// Current uplink processing slot context.
  slot_point ul_slot_context = {};
  /// Current downlink processing slot context.
  slot_point dl_slot_context = {};
  /// Indicates the asynchronous processing shall stop.
  std::atomic<bool> quit = {};

  /// \brief Processes uplink symbol.
  /// \param[in] symbol_idx Indicates the symbol index within a subframe.
  /// \return The radio timestamp of the received block.
  radio_timestamp process_ul_symbol(unsigned symbol_idx);

  /// \brief Processes downlink symbol.
  /// \param[in] symbol_idx Indicates the symbol index within a subframe.
  /// \param[out] timestamp Indicates the radio timestamp for transmitting the symbol.
  void process_dl_symbol(unsigned symbol_idx, radio_timestamp timestamp);

  /// \brief Processes uplink and downlink slot.
  void process_slot();

  /// \brief Runs the lower physical layer instance.
  /// \param[in] realtime_task_executor Provides the real time task executor.
  void run(task_executor& realtime_task_executor);

  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override;

public:
  /// \brief Constructs a generic lower physical layer.
  /// \param[in] config Provides the necessary parameters to construct the lower physical layer.
  lower_phy_impl(const lower_phy_configuration& config);

  // See interface for documentation.
  lower_phy_input_gateway& get_input_gateway() override { return *this; }

  // See interface for documentation.
  void start(task_executor& realtime_task_exeutor) override;

  void stop() override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_LOWER_LOWER_PHY_IMPL_H
