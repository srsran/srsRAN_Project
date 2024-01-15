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

#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include <memory>

namespace srsran {

/// Describes a generic implementation of a DMRS for PDCCH processor.
class dmrs_pdcch_processor_impl : public dmrs_pdcch_processor
{
private:
  /// DM-RS index increment in frequency domain.
  static constexpr unsigned STRIDE = 4;

  /// Number of DM-RS for PDCCH per active resource block.
  static constexpr unsigned NOF_DMRS_PER_RB = NRE / STRIDE;

  /// Maximum number of DM-RS for PDCCH that can be found in a symbol.
  static constexpr unsigned MAX_NOF_DMRS_PER_SYMBOL = pdcch_constants::MAX_NOF_RB_PDCCH * NOF_DMRS_PER_RB;

  /// Maximum number of DM-RS for PDCCH that can be found in a transmission.
  static constexpr unsigned MAX_NOF_DMRS = pdcch_constants::MAX_CORESET_DURATION * MAX_NOF_DMRS_PER_SYMBOL;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg;

  /// Temporary sequence storage.
  std::array<cf_t, MAX_NOF_DMRS_PER_SYMBOL> temp_sequence;

  /// \brief Computes the initial pseudo-random state.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Required parameters.
  /// \return The initial pseudo-random state.
  static unsigned c_init(unsigned symbol, const config_t& config);

  /// \brief Sequence generation as per TS 38.211 Section 7.4.1.3.1.
  ///
  /// This method generates the sequence described in TS 38.211 Section 7.4.1.3.1, considering the only values required
  /// to fill the resource blocks according to TS 38.211 Section 7.3.2.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Mapping to physical resources as per TS 38.211 Section 7.4.1.3.2.
  ///
  /// This method implements the signal mapping as described in TS 38.211 Section 7.4.1.3.2.
  ///
  /// \param[out] mapper          Resource grid mapper interface.
  /// \param[in] d_pdcch          PDCCH resource elements to map in the resource grid.
  /// \param[in] config           PDCCH modulator parameters.
  void mapping(resource_grid_mapper& mapper, const re_buffer_reader& d_pdcch, const config_t& config);

public:
  explicit dmrs_pdcch_processor_impl(std::unique_ptr<pseudo_random_generator> prg_) : prg(std::move(prg_))
  {
    srsran_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_mapper& grid, const config_t& config) override;
};

} // namespace srsran
