/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"

namespace srsgnb {

/// \brief Describes a PDSCH modulator interface.
///
/// The PDSCH modulator shall modulate PDSCH according to TS38.211 section 7.3.1 Physical downlink shared channel. It
/// provides two possible modulation granularities:
/// - Codeword: it modulates a codeword and maps it.
/// - Codeblock: it modulates a single codeblock from a codeword.
class pdsch_modulator
{
public:
  /// Defines the maximum number of codewords per PDSCH transmission.
  static constexpr unsigned MAX_NOF_CODEWORDS = 2;

  /// Defines the maximum number of RE per codeword in a PDSCH transmission.
  static constexpr unsigned MAX_CODEWORD_SIZE = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_PORTS / 2;

  /// Describes the necessary parameters to modulate a PDSCH transmission.
  struct config_t {
    /// Provides \f$n_{RNTI}\f$ from TS38.211 section 7.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB.
    unsigned bwp_start_rb;
    /// Modulation of codeword 1 (q = 0).
    modulation_scheme modulation1;
    /// Modulation of codeword 2 ( q = 1).
    modulation_scheme modulation2;
    /// Frequency domain allocation.
    rb_allocation freq_allocation;
    /// Time domain allocation within a slot: start symbol index (0...12).
    unsigned start_symbol_index;
    /// Time domain allocation within a slot: number of symbols (1...14).
    unsigned nof_symbols;
    /// DMRS symbol positions as a mask.
    std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symb_pos;
    /// Indicates the DMRS configuration type.
    dmrs_type dmrs_config_type;
    /// Number of DMRS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Provides \f$n_{ID}\f$ from TS38.211 section 7.3.1.1 Scrambling. Range is (0...1023).
    unsigned n_id;
    /// Scaling factor to apply to the resource elements according to PDSCH power allocation in TS38.214.
    float scaling;
    /// Reserved RE pattern where PDSCH is not mapped.
    re_pattern_list reserved;
    /// Precoding matrix indicator.
    unsigned pmi;
    /// Port indexes the PDSCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~pdsch_modulator() = default;

  /// \brief Modulates a PDSCH codeword according to TS38.211 section 7.3.1 Physical downlink shared channel.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] codewords Provides the encoded codewords to modulate.
  /// \param[in] config Provides the configuration reference.
  /// \note The number of codewords shall be consistent with the number of layers.
  /// \note The codeword length shall be consistent with the resource mapping, considering the reserved resource
  /// elements.
  virtual void
  modulate(resource_grid_writer& grid, span<const span<const uint8_t> > codewords, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_H
