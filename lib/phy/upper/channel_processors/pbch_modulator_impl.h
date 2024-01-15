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

#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pbch_modulator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsran {

class pbch_modulator_impl : public pbch_modulator
{
private:
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// Implements TS 38.211 section 7.3.3.1 Scrambling
  /// \param [in] b Inputs bits to scramble
  /// \param [out] b_hat Output bits after scrambling
  /// \param [in] config PBCH modulator arguments
  void scramble(span<const uint8_t> b, span<uint8_t> b_hat, const config_t& config);

  /// Implements TS 38.211 section 7.3.3.2 Modulation
  /// \param [in] b_hat Inputs bits to scramble
  /// \param [out] d_pbch Output symbols
  void modulate(span<const uint8_t> b_hat, span<cf_t> d_pbch);

  /// Implements TS 38.211 section 7.3.3.3 Mapping to physical resources
  /// \param [in] d_pbch Provides the symbols to map
  /// \param [in, out] grid is the destination resource grid
  /// \param [in] config PBCH modulator arguments
  void map(span<const cf_t> d_pbch, resource_grid_writer& grid, const config_t& config);

public:
  pbch_modulator_impl(std::unique_ptr<modulation_mapper> modulator_, std::unique_ptr<pseudo_random_generator> prg_) :
    modulator(std::move(modulator_)), scrambler(std::move(prg_))
  {
    srsran_assert(modulator, "Invalid modulator.");
    srsran_assert(scrambler, "Invalid scrambler.");
  }

  void put(span<const uint8_t> bits, resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsran
