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
/// \brief Encoder interface for short blocks.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ran/sch/modulation_scheme.h"

#include <memory>

namespace srsran {

// TODO(david,borja): These will probably need to be moved somewhere else since they must be accessible by the
// scrambler.
/// \brief UCI placeholder bit.
///
/// A placeholder for setting a bit to 1 in the scrambler. It is denoted as "\c x" in TS38.212 Section 5.3.3 and in
/// TS38.211 Section 6.3.1.1.
constexpr uint8_t PLACEHOLDER_ONE = 255;
/// \brief UCI placeholder bit.
///
/// A placeholder for repeating the previous bit in the scrambler. It is denoted as "\c y" in TS38.212 Section 5.3.3 and
/// in TS38.211 Section 6.3.1.1.
constexpr uint8_t PLACEHOLDER_REPEAT = 254;

/// Short-block encoder interface.
class short_block_encoder
{
public:
  /// Default destructor.
  virtual ~short_block_encoder() = default;

  /// \brief Encodes and rate-matches a small number (less than 12) bits.
  ///
  /// Encodes a short sequence of bits (length 1 to 11, both included), following the specifications of TS38.212
  /// Section 5.3.3, and rate-matches the encoded bits according to TS38.212 Section 5.4.3.
  /// \param[out] output Sequence of encoded and rate-matched bits (i.e., \f$f_0, f_1, \dots, f_{E-1}\f$ in TS38.212).
  /// \param[in]  input  Sequence of bits to encode (i.e., \f$c_0, c_1, \dots, c_{K-1}\f$ in TS38.212).
  /// \param[in]  mod    Modulation scheme.
  /// \remark Both \c output and \c input follow an unpacked representation, that is one bit per entry.
  /// \remark The size \f$E\f$ of \c output should be as follows:
  ///   - \f$E \ge Q_m\f$, where \f$Q_m\f$ is the modulation order, if \c input is a single bit;
  ///   - \f$E \ge 3Q_m\f$ if \c input consists of 2 bits;
  ///   - \f$E \ge 32\f$ otherwise (\c input size from 3 to 11 bits).
  virtual void encode(span<uint8_t> output, span<const uint8_t> input, modulation_scheme mod) = 0;
};

std::unique_ptr<short_block_encoder> create_short_block_encoder();
} // namespace srsran
