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

#include "srsran/adt/span.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include <memory>

namespace srsran {

/// Describes the PDCCH encoder interface.
class pdcch_encoder
{
public:
  /// Describes the necessary parameters to encode PDCCH message.
  struct config_t {
    /// Number of rate-matched bits.
    unsigned E;
    /// RNTI used for CRC bits scrambling according to TS 38.212 section 7.3.2.
    unsigned rnti;
  };

  /// Default destructor.
  virtual ~pdcch_encoder() = default;

  /// \brief Encodes a PDCCH message.
  /// \param [in]  data    - unencoded message bits
  /// \param [out] encoded - encoded bits
  /// \param [in]  config  - encoder configuration
  ///
  /// \remark size of + 24 bits of CRC gives K parameter for the Polar encoder

  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsran
