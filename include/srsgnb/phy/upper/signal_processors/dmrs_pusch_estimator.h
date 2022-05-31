/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUSCH_ESTIMATOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUSCH_ESTIMATOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimate.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes a DMRS for PUSCH channel estimator interface.
class dmrs_pusch_estimator
{
public:
  /// Describes the required parameters to receive the signal described in 3GPP TS38.211 section 6.4.1.1.
  struct config_t {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PUSCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DL DMRS config type (dmrsConfigType).
    dmrs_type type;
    /// PUSCH DMRS-Scrambling-ID (puschDmrsScramblingId).
    unsigned scrambling_id;
    /// DMRS sequence initialization (\f$n_{SCID}\f$).
    bool n_scid;
    /// Indicates the generated signal linear amplitude.
    float amplitude;
    /// DMRS symbol position indexes.
    std::array<bool, MAX_NSYMB_PER_SLOT> symbols_mask;
    /// Allocation RB list, the entries set to true are used for transmission.
    bounded_bitset<MAX_RB> rb_mask;
    /// List of ports, every entry is an index.
    static_vector<uint8_t, DMRS_MAX_NPORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pusch_estimator() = default;

  /// \brief Channel estimation based on DMRS for PUSCH.
  /// \param[out] estimate Channel estimate.
  /// \param[in] grid Provides the source resource grid.
  /// \param[in] config Provides the required configuration to generate and map the signal.
  virtual void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const config_t& config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUSCH_ESTIMATOR_H
