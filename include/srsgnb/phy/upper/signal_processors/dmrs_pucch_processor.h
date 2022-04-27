#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/pucch_mapping.h"
#include <memory>

namespace srsgnb {

/// Describes a DMRS for PUCCH processor interface.
class dmrs_pucch_processor
{
public:
  /// Describes the necessary parameters to generate DMRS for a PUCCH transmission.
  struct config_t {
    /// Specifies PUCCH format this configuration belongs.
    pucch_format format;
    /// Provides the slot timing and numerology.
    slot_point slot;
    /// Configuration of group and sequence hopping.
    pucch_group_hopping group_hopping;
    /// Start symbol index, see TS 38.331 PUCCH-Resource IE.
    unsigned start_symbol_index;
    /// Number of symbols, see TS 38.331 PUCCH-Resource IE.
    unsigned nof_symbols;
    /// Start PRB index, common to all formats (see TS 38.331 PUCCH-Resource IE).
    unsigned starting_prb;
    /// Enabling intra-slot frequency hopping, applicable for all PUCCH formats (see PUCCH-Resource IE in TS 38.331).
    bool intra_slot_hopping;
    /// Index of first PRB after frequency hopping of PUCCH (see PUCCH-Resource IE in TS 38.331).
    unsigned second_hop_prb;
    /// Number of PRBs, applicable for formats 2 and 3 (see PUCCH-Resource IE in TS 38.331).
    unsigned nof_prb;
    /// Initial cyclic shift, used by formats 0 and 1 as defined in TS 38.211 subclause 6.3.2.2.2.
    unsigned initial_cyclic_shift;
    /// Orthogonal covering code index, used by format 1 (see PUCCH-Resource IE in TS 38.331).
    unsigned time_domain_occ;
    /// UE enables 2 DMRS symbols per hop of a PUCCH Format 3 or 4 (see PUCCH-Format-Config IE in TS 38.331).
    bool additional_dmrs;
    /// Higher layer parameter hopingID if configured (Cell-specific scrambling ID for group hopping and sequence
    /// hopping), otherwise the physical cell identifier, as described in TS 38.211 6.3.2.2.2.
    unsigned n_id;
    /// DMRS scrambling identity, defined in TS 38.211 subclause 6.4.1.3.2.1;
    /// Higher layer parameter scramblingID0 in DMRS-UplinkConfig if it is given, otherwise the physical cell
    /// identifier.
    unsigned n_id_0;
  };

  /// Default destructor.
  virtual ~dmrs_pucch_processor() = default;

  /// \brief Generates and maps DMRS for PUCCH according to TS 38.211 section 6.4.1.3.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] config Provides the required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_H
