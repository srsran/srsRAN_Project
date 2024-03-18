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

#pragma once

#include "srsran/phy/constants.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include <memory>

namespace srsran {

/// Implements a generic Sounding Reference Signal estimator.
class srs_estimator_generic_impl : public srs_estimator
{
public:
  /// Collects the component dependencies.
  struct dependencies {
    /// Sequence generator.
    std::unique_ptr<low_papr_sequence_generator> sequence_generator;
  };

  srs_estimator_generic_impl(dependencies deps_) : deps(std::move(deps_))
  {
    srsran_assert(deps.sequence_generator, "Invalid sequence generator.");
  }

  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override;

private:
  /// \brief Maximum sequence length in subcarriers.
  ///
  /// It is given by the maximum value of \f$m_{SRS,0}\f$ in TS38.211 Table 6.4.1.4.3-1 and a comb size of 2.
  static constexpr unsigned max_seq_length = 272 * NRE / 2;

  /// \brief Extract the received Sounding Reference Signals sequence from a resource grid.
  ///
  /// The sequence mapping is given in TS38.211 Section 6.4.1.4.3.
  ///
  /// \param[out] sequence           Extracted sequence.
  /// \param[in]  grid               Received resource grid.
  /// \param[in]  i_rx_port          Receive port index.
  /// \param[in]  i_symbol           OFDM symbol index within the slot to extract.
  /// \param[in]  initial_subcarrier Initial subcarrier index relative to Point A. Parameter \f$k_0^{(p_i)}\f$.
  /// \param[in]  comb_size          Subcarrier stride. Parameter \f$K_{TC}\f$.
  /// \return A vector containing the sequence.
  void extract_sequence(span<cf_t>                  sequence,
                        const resource_grid_reader& grid,
                        unsigned                    i_rx_port,
                        unsigned                    i_symbol,
                        unsigned                    initial_subcarrier,
                        unsigned                    comb_size);

  /// Dependencies collection.
  dependencies deps;
};

} // namespace srsran