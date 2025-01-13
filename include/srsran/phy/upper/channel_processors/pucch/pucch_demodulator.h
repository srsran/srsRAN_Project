/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

namespace srsran {

class resource_grid_reader;

/// PUCCH demodulator interface for Formats 2, 3 and 4.
class pucch_demodulator
{
public:
  /// Collects PUCCH Format 2 demodulation parameters.
  struct format2_configuration {
    /// Port indices used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
    /// Lowest PRB index used for the PUCCH transmission within the resource grid {0, ..., 274}.
    unsigned first_prb;
    /// \brief Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if intra-slot frequency hopping is
    /// enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Number of PRB allocated to PUCCH Format 2 {1, ..., 16}.
    unsigned nof_prb;
    /// Start symbol index within the slot {0, ..., 13}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {1, 2}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1. Range is {0, ..., 1023}.
    unsigned n_id;
  };

  /// Collects PUCCH Format 3 demodulation parameters.
  struct format3_configuration {
    /// Port indices used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
    /// Lowest PRB index used for the PUCCH transmission within the resource grid {0, ..., 274}.
    unsigned first_prb;
    /// \brief Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if intra-slot frequency hopping is
    /// enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Number of PRB allocated to PUCCH Format 3 {1, ..., 16}.
    unsigned nof_prb;
    /// Start symbol index within the slot {0, ..., 13}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1. Range is {0, ..., 1023}.
    unsigned n_id;
    /// Set to higher layer parameter \e additionalDMRS described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool additional_dmrs;
    /// Set to higher layer parameter \e pi2BPSK described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool pi2_bpsk;
  };

  /// Collects PUCCH Format 4 demodulation parameters.
  struct format4_configuration {
    /// Port indices used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
    /// Lowest PRB index used for the PUCCH transmission within the resource grid {0, ..., 274}.
    unsigned first_prb;
    /// \brief Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if intra-slot frequency hopping is
    /// enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Start symbol index within the slot {0, ..., 13}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1. Range is {0, ..., 1023}.
    unsigned n_id;
    /// Set to higher layer parameter \e additionalDMRS described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool additional_dmrs;
    /// Set to higher layer parameter \e pi2BPSK described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool pi2_bpsk;
    /// Set to higher layer parameter \e occ-Length described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-format4.
    unsigned occ_length;
    /// Set to higher layer parameter \e occ-Index described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-format4.
    unsigned occ_index;
  };

  /// Default destructor.
  virtual ~pucch_demodulator() = default;

  /// \brief Demodulates a PUCCH Format 2 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Channel estimates.
  /// \param[in]  config    PUCCH Format 2 configuration parameters.
  virtual void demodulate(span<log_likelihood_ratio>   llr,
                          const resource_grid_reader&  grid,
                          const channel_estimate&      estimates,
                          const format2_configuration& config) = 0;

  /// \brief Demodulates a PUCCH Format 3 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Channel estimates.
  /// \param[in]  config    PUCCH Format 3 configuration parameters.
  virtual void demodulate(span<log_likelihood_ratio>   llr,
                          const resource_grid_reader&  grid,
                          const channel_estimate&      estimates,
                          const format3_configuration& config) = 0;

  /// \brief Demodulates a PUCCH Format 4 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Channel estimates.
  /// \param[in]  config    PUCCH Format 4 configuration parameters.
  virtual void demodulate(span<log_likelihood_ratio>   llr,
                          const resource_grid_reader&  grid,
                          const channel_estimate&      estimates,
                          const format4_configuration& config) = 0;
};

} // namespace srsran
