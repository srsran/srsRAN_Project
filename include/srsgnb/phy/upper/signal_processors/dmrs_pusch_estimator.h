/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH channel estimator interface declaration.

#pragma once

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimate.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// DM-RS-based PUSCH channel estimator: interface.
class dmrs_pusch_estimator
{
public:
  /// Parameters required to receive the demodulation reference signals described in 3GPP TS38.211 Section 6.4.1.1.
  struct configuration {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PUSCH DM-RS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DL DM-RS configuration type.
    dmrs_type type;
    /// PUSCH DM-RS scrambling ID.
    unsigned scrambling_id;
    /// DM-RS sequence initialization (parameter \f$n_{SCID}\f$ in the TS).
    bool n_scid;
    /// Generated signal linear amplitude.
    float amplitude;
    /// DM-RS symbol position indices.
    std::array<bool, MAX_NSYMB_PER_SLOT> symbols_mask;
    /// Allocation RB list: the entries set to true are used for transmission.
    bounded_bitset<MAX_RB> rb_mask;
    /// List of ports: every entry is an index.
    static_vector<uint8_t, DMRS_MAX_NPORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pusch_estimator() = default;

  /// \brief Estimates the PUSCH propagation channel.
  /// \param[out] estimate Channel estimate.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   DM-RS configuration parameters. They characterize the DM-RS symbols and their indices.
  virtual void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) = 0;
};

} // namespace srsgnb
