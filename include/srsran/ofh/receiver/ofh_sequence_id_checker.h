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

#include <cstdint>

namespace srsran {
namespace ofh {

/// Open Fronthaul sequence identifier checker.
class sequence_id_checker
{
public:
  /// Default destructor.
  virtual ~sequence_id_checker() = default;

  /// \brief Updates the expected sequence identifier value for the given eAxC and compares it with the given sequence
  /// identifier, returning the difference between them.
  ///
  /// A negative difference means that the sequence identifier is from the past.
  /// A difference of 0 means that the current sequence identifier matches the expected.
  /// A positive difference means that the sequence identifier belongs to the future. In this case, the expected
  /// sequence identifier is updated with the given sequence identifier.
  virtual int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id) = 0;
};

} // namespace ofh
} // namespace srsran
