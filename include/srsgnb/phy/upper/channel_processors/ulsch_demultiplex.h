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

#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/modulation_scheme.h"

/// \file
/// \brief Uplink Shared Channel demultiplexer interface declaration.

namespace srsgnb {

/// \brief Uplink Shared Channel demultiplexer interface.
///
/// User interface of the Uplink Shared Channel (UL-SCH) demultiplexer.
class ulsch_demultiplex
{
public:
  /// Collects the necessary parameters for performing the UL-SCH demultiplex according to TS38.212 Section 6.2.7.
  struct configuration {
    /// Modulation scheme.
    modulation_scheme modulation;
    /// Number of layers {1, ..., 4}
    unsigned nof_layers;
    /// Number of Physical Resource Blocks allocated to the UL-SCH transmission.
    unsigned nof_prb;
    /// Index of the first allocated OFDM symbol within the slot.
    unsigned start_symbol_index;
    /// Number of allocated OFDM symbols.
    unsigned nof_symbols;
    /// Number of reserved bits for HARQ-ACK. Parameter \f$G^\mathrm{HARQ-ACK}_\mathrm{rvd}\f$.
    unsigned nof_harq_ack_rvd;
    /// Transmission DM-RS Type.
    dmrs_type dmrs;
    /// Mask indicating which OFDM symbols in the slot contain DM-RS.
    bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask;
    /// Number of CDM groups without data.
    unsigned nof_cdm_groups_without_data;
  };

  /// Default destructor.
  virtual ~ulsch_demultiplex() = default;

  /// \brief Demultiplexes Uplink Shared Channel (UL-SCH).
  ///
  /// Demultiplexes the different information fields from the UL-SCH transmission.
  ///
  /// \param[out] sch_data   Destination of the resultant shared channel data soft bits.
  /// \param[out] harq_ack   Destination of HARQ-ACK information soft bits.
  /// \param[out] csi_part1  Destination CSI Part1 report soft bits.
  /// \param[out] csi_part2  Destination CSI Part2 report soft bits.
  /// \param[in] input       Input soft bits to demultiplex.
  /// \param[in] config      UL-SCH demultiplexing parameters.
  virtual void demultiplex(span<log_likelihood_ratio>       sch_data,
                           span<log_likelihood_ratio>       harq_ack,
                           span<log_likelihood_ratio>       csi_part1,
                           span<log_likelihood_ratio>       csi_part2,
                           span<const log_likelihood_ratio> input,
                           const configuration&             config) = 0;
};

} // namespace srsgnb
