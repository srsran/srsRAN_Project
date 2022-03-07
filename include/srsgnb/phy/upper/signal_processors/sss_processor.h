#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

/// Describes a SSS processor interface
class sss_processor
{
public:
  /// Describes the required parameters to generate the signal
  struct config_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// Denotes the first symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// SSS linear signal amplitude
    float amplitude;
    /// Port indexes to map the signal.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor
  virtual ~sss_processor() = default;

  /// \brief Generates and maps a SSS sequence
  /// \param [out] grid Provides the destination resource grid
  /// \param [in] config Provides the required configuration to generate and map the signal
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Creates a generic SSS processor instance
std::unique_ptr<sss_processor> create_sss_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H
