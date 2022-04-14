#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/ran/dmrs_mapping.h"

namespace srsgnb {

/// \brief Describes a PDCCH modulator interface.
///
/// \remark Defined in TS 38.211 section 7.3.2 Physical downlink control channel (PDCCH).
/// \note The configuration assumes the CCE-to-REG and REG-to-PRB mapping is included in \c rb_mask.
class pdcch_modulator
{
public:
  /// Describes the necessary parameters to modulate a PDCCH transmission.
  struct config_t {
    /// Indicates the RBs used for the PDCCH transmission.
    std::array<bool, MAX_RB> rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling according to TS 38.211 section 7.3.2.3.
    unsigned n_rnti;
    /// Scaling factor to apply to the resource elements according to PDCCH power allocation in TS 38.213.
    float scaling;
    /// Port indexes the PDSCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~pdcch_modulator() = default;

  /// \brief Modulates a PDCCH codeword according to TS 38.211 section 7.3.2 Physical downlink control channel.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] data Provides the encoded and unpacked bits to modulate.
  /// \param[in] config Provides the configuration.
  /// \note The codeword length shall be consistent with the resource mapping.
  virtual void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_H
