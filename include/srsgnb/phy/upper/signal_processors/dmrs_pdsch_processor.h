#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/dmrs_mapping.h"
#include "srsgnb/ran/slot_point.h"
#include <memory>

namespace srsgnb {

/// Describes a DMRS for PDSCH processor interface.
class dmrs_pdsch_processor
{
public:
  /// Describes the required parameters to generate the signal as described in 3GPP TS 38.211 section 7.4.1.1.
  struct config_t {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PDSCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DL DMRS config type (dmrsConfigType).
    dmrs_type type;
    /// PDSCH DMRS-Scrambling-ID (pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DMRS sequence initialization (nSCID).
    bool n_scid;
    /// Number of DMRS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Precoding matrix indicator, set to 0 for no precoding.
    unsigned pmi;
    /// DMRS symbol position indexes.
    std::array<bool, MAX_NSYMB_PER_SLOT> symbols_mask;
    /// Allocation RB list, the entries set to true are used for transmission.
    std::array<bool, MAX_RB> rb_mask;
    /// List of ports, every entry is an index.
    static_vector<unsigned, DMRS_MAX_NPORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pdsch_processor() = default;

  /// \brief Generates and maps DMRS for PDSCH.
  /// \param [out] grid Provides the destination resource grid.
  /// \param [in] config Provides the required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Creates a generic DMRS for PDSCH instance
std::unique_ptr<dmrs_pdsch_processor> create_dmrs_pdsch_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_H
