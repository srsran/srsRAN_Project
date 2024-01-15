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
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsran {

class resource_grid_mapper;

class pdcch_modulator_impl : public pdcch_modulator
{
private:
  /// Defines the maximum number of resource blocks used in a transmission. It considers the maximum aggregation level
  /// of 16 and a REG size of 6 RB.
  static constexpr unsigned MAX_RB_PER_PDCCH = 16 * 6;

  /// Defines the number of resource elements available for PDCCH in a resource block.
  static constexpr unsigned NOF_RE_PER_RB = NRE - 3;

  /// Defines the maximum number of resource elements that can be utilised for a PDCCH transmission.
  static constexpr unsigned MAX_RE = NOF_RE_PER_RB * MAX_RB_PER_PDCCH;

  /// Defines the maximum number of bits that can be utilised for a PDCCH transmission.
  static constexpr unsigned MAX_BITS = MAX_RE * get_bits_per_symbol(modulation_scheme::QPSK);

  /// Provides an implementation of the modulator.
  std::unique_ptr<modulation_mapper> modulator;

  /// Provides an implementation of the the pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;

  /// \brief Scrambles a codeword. Implements TS 38.211 section 7.3.2.3 Scrambling.
  ///
  /// \param[out] b_hat Output bits after scrambling.
  /// \param[in] b Inputs bits to scramble.
  /// \param[in] config Provides the scrambler configuration.
  void scramble(span<uint8_t> b_hat, span<const uint8_t> b, const config_t& config);

  /// \brief Modulates a codeword. Implements TS 38.211 section 7.3.2.4 PDCCH modulation.
  ///
  /// \param[out] d_pdcch Output symbols.
  /// \param[in] b_hat Inputs bits to perform the modulation mapping.
  /// \param[in] scaling Indicates the signal scaling if the value is valid (not 0, NAN nor INF).
  void modulate(span<cf_t> d_pdcch, span<const uint8_t> b_hat, float scaling);

  /// \brief Maps the modulated symbols to resource elements in the grid. Implements TS 38.211 section 7.3.2.5 Mapping
  /// to physical resources.
  ///
  /// \param[out] mapper   Provides the destination resource grid.
  /// \param[in] d_pdcch Provides the block of complex-valued symbols to map.
  /// \param[in] config  Provides the mapper configuration.
  void map(resource_grid_mapper& mapper, const re_buffer_reader& d_pdcch, const config_t& config);

public:
  // See interface for the documentation.
  void modulate(resource_grid_mapper& grid, span<const uint8_t> data, const config_t& config) override;

  /// Generic PDCCH modulator instance constructor.
  pdcch_modulator_impl(std::unique_ptr<modulation_mapper>       modulator_,
                       std::unique_ptr<pseudo_random_generator> scrambler_) :
    modulator(std::move(modulator_)), scrambler(std::move(scrambler_))
  {
    srsran_assert(modulator, "Invalid modulator");
    srsran_assert(scrambler, "Invalid scrambler");
  }
};

} // namespace srsran
