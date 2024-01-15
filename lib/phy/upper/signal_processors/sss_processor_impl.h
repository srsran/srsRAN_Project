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
#include "srsran/phy/upper/signal_processors/sss_processor.h"

namespace srsran {
class sss_processor_impl : public sss_processor
{
private:
  /// First subcarrier in the SS/PBCH block
  static const unsigned SSB_K_BEGIN = 56;
  /// Symbol index in the SSB where the SSS is mapped
  static const unsigned SSB_L = 2;
  /// SSS Sequence length in the SSB
  static const unsigned SEQUENCE_LEN = 127;

  struct pregen_d0_s : public std::array<cf_t, SEQUENCE_LEN> {
    pregen_d0_s();
  };

  struct pregen_d1_s : public std::array<cf_t, SEQUENCE_LEN> {
    pregen_d1_s();
  };

  static const pregen_d0_s d0;
  static const pregen_d1_s d1;

  /// \brief Implements TS 38.211 section 7.4.2.3.1 Sequence generation
  /// \param [out] sequence provides the destination of the sequence
  /// \param [in] config provides the necessary parameters to generate the sequence
  void generation(std::array<cf_t, SEQUENCE_LEN>& sequence, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.2.3.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the destination of the sequence mapping
  /// \param [in] config provides the necessary parameters to generate the sequence
  void
  mapping(const std::array<cf_t, SEQUENCE_LEN>& sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsran
