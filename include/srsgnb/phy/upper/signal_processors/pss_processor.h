#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

/// Describes a PSS processor interface
class pss_processor
{
public:
  /// Describes the required parameters to generate the signal
  struct config_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// First symbol in slot resource grid
    unsigned ssb_first_symbol;
    /// PSS linear signal amplitude
    float amplitude;
    /// Ports indexes to map the signal
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor
  virtual ~pss_processor() = default;

  /// \brief Generates and maps a PSS sequence
  /// \param [out] grid provides the destination resource grid
  /// \param [in] config provides the required configuration to generate and map the signal
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Creates a generic PSS processor instance
std::unique_ptr<pss_processor> create_pss_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
