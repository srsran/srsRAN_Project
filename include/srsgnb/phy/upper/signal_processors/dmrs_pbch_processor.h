#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

/// Describes a DMRS for PBCH processor interface
class dmrs_pbch_processor
{
public:
  /// Describes the required parameters to generate the signal
  struct config_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// SS/PBCH block index in the burst
    unsigned ssb_idx;
    /// Maximum number of SS/PBCH transmissions in a burst (5ms)
    unsigned L_max;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// Denotes the first symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// Half frame flag
    bool n_hf;
    /// Linear signal amplitude
    float amplitude;
    /// Port indexes to map the signal.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor
  virtual ~dmrs_pbch_processor() = default;

  /// \brief Generates and maps DMRS for PBCH
  /// \param [out] grid provides the destination resource grid
  /// \param [in] config provides the required configuration to generate and map the signal
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Creates a generic DMRS for PBCH instance
std::unique_ptr<dmrs_pbch_processor> create_dmrs_pbch_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
