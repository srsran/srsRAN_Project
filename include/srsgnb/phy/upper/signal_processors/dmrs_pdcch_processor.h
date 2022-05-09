#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"
#include <memory>

namespace srsgnb {

/// Describes a DMRS for PDCCH processor interface.
class dmrs_pdcch_processor
{
public:
  /// Describes the necessary parameters to generate DMRS for a PDCCH transmission.
  struct config_t {
    /// Provides the slot timing and numerology.
    slot_point slot;
    /// Indicates the cyclic prefix.
    cyclic_prefix cp;
    /// Reference point for PDCCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// Indicates the RBs used for the PDCCH transmission.
    std::array<bool, MAX_RB> rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Provides the linear signal amplitude to conform with the transmission power.
    float amplitude;
    /// Port indexes the PDSCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pdcch_processor() = default;

  /// \brief Generates and maps DMRS for PDCCH according to TS 38.211 section 7.4.1.3.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] config Provides the required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Creates a generic DMRS for PDCCH instance
std::unique_ptr<dmrs_pdcch_processor> create_dmrs_pdcch_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_H
