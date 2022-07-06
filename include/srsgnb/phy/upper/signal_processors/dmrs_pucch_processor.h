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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/pucch_mapping.h"
#include "srsgnb/ran/slot_point.h"
#include <memory>

namespace srsgnb {

// forward declarations
class pucch_orthogonal_sequence;
class low_papr_sequence_collection;

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
    /// Indicates the cyclic prefix.
    cyclic_prefix cp;
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
    /// Port indexes the PDSCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pucch_processor() = default;

  /// \brief Generates and maps DMRS for PUCCH according to TS 38.211 section 6.4.1.3.
  ///
  /// \param[out] estimate Channel estimation results.
  /// \param[in]  grid     Provides the source resource grid.
  /// \param[in]  config   Provides the required configuration to estimate channel.
  virtual void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const config_t& config) = 0;
};

/// Creates a generic DMRS for PUCCH instance.
std::unique_ptr<dmrs_pucch_processor> create_dmrs_pucch_processor(const pucch_format&                 format,
                                                                  const low_papr_sequence_collection* c   = nullptr,
                                                                  const pucch_orthogonal_sequence*    occ = nullptr);

} // namespace srsgnb
