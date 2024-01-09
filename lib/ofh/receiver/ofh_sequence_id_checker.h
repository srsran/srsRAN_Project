/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
