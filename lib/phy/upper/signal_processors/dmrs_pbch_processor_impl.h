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

#include "srsran/adt/complex.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pbch_processor.h"

namespace srsran {

class dmrs_pbch_processor_impl : public dmrs_pbch_processor
{
private:
  /// DMRS PBCH Sequence length in the SSB
  static const unsigned NOF_RE = 144;

  std::unique_ptr<pseudo_random_generator> prg;

  /// \brief Computes the initial pseudo-random state
  /// \param [in] config provides the required parameters to calculate the value
  /// \return the initial pseudo-random state
  static unsigned c_init(const config_t& config);

  /// \brief Implements TS 38.211 section 7.4.1.4.1 Sequence generation
  /// \param [out] sequence provides the destination of the sequence generation
  /// \param [in] config provides the required fields to generate the signal
  void generation(std::array<cf_t, NOF_RE>& sequence, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.1.4.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the grid destination to map the signal
  /// \param [in] config provides the required fields to map the signal
  void mapping(const std::array<cf_t, NOF_RE>& sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  dmrs_pbch_processor_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_generator) :
    prg(std::move(pseudo_random_generator))
  {
    srsran_assert(prg, "Invalid PRG.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsran
