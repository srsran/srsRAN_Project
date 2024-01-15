/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief PUSCH channel estimator interface declaration.

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

class resource_grid_reader;

/// DM-RS-based PUSCH channel estimator interface.
class dmrs_pusch_estimator
{
public:
  /// Parameters required to receive the demodulation reference signals described in 3GPP TS38.211 Section 6.4.1.1.
  struct configuration {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// DL DM-RS configuration type.
    dmrs_type type;
    /// PUSCH DM-RS scrambling ID.
    unsigned scrambling_id;
    /// DM-RS sequence initialization (parameter \f$n_{SCID}\f$ in the TS).
    bool n_scid;
    /// \brif DM-RS amplitude scaling factor.
    ///
    /// Parameter \f$\beta _{\textup{PUSCH}}^{\textup{DMRS}}\f$ as per TS38.211 Section 6.4.1.1.3. It must be set
    /// to \f$\beta _{\textup{PUSCH}}^{\textup{DMRS}}=10^{-\beta_{\textup{DMRS}}/20}\f$, as per TS38.214
    /// Section 6.2.2, where \f$\beta_{\textup{DMRS}}\f$ is the PUSCH EPRE to DM-RS EPRE ratio expressed in decibels, as
    /// specified in TS38.214 Table 6.2.2-1.
    ///
    /// \sa get_sch_to_dmrs_ratio_dB()
    float scaling;
    /// Cyclic prefix.
    cyclic_prefix c_prefix = cyclic_prefix::NORMAL;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    bounded_bitset<MAX_NSYMB_PER_SLOT> symbols_mask;
    /// Allocation RB list: the entries set to true are used for transmission.
    bounded_bitset<MAX_RB> rb_mask;
    /// First OFDM symbol within the slot for which the channel should be estimated.
    unsigned first_symbol = 0;
    /// Number of OFDM symbols for which the channel should be estimated.
    unsigned nof_symbols = 0;
    /// Number of transmit layers.
    unsigned nof_tx_layers = 0;
    /// List of receive ports.
    static_vector<uint8_t, DMRS_MAX_NPORTS> rx_ports;
  };

  /// Default destructor.
  virtual ~dmrs_pusch_estimator() = default;

  /// \brief Estimates the PUSCH propagation channel.
  /// \param[out] estimate Channel estimate.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   DM-RS configuration parameters. They characterize the DM-RS symbols and their indices.
  virtual void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) = 0;
};

} // namespace srsran
