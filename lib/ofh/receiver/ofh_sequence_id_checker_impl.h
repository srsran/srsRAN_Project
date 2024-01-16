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

#include "ofh_sequence_id_checker.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/circular_map.h"
#include "srsran/ofh/ofh_constants.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sequence identifier checker implementation.
class sequence_id_checker_impl : public sequence_id_checker
{
  static constexpr int NOF_SEQUENCES_IDENTIFIERS      = 1u << 8;
  static constexpr int HALF_NOF_SEQUENCES_IDENTIFIERS = NOF_SEQUENCES_IDENTIFIERS / 2;

  bounded_bitset<MAX_SUPPORTED_EAXC_ID_VALUE>                  initialized;
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> counters;

public:
  /// Default constructor.
  sequence_id_checker_impl() : initialized(MAX_SUPPORTED_EAXC_ID_VALUE)
  {
    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      counters.insert(K, 0);
    }
  }

  // See interface for documentation.
  int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id) override
  {
    srsran_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE,
                  "Invalid eAxC value '{}'. Maximum eAxC value is '{}'",
                  eaxc,
                  MAX_SUPPORTED_EAXC_ID_VALUE);

    // First packet is always valid.
    if (!initialized.test(eaxc)) {
      initialized.set(eaxc);
      counters[eaxc] = seq_id;

      return 0;
    }

    // Get the expected sequence identifier and update its value.
    uint8_t expected_seq_id = counters[eaxc] + 1;

    if (seq_id == expected_seq_id) {
      ++counters[eaxc];

      return 0;
    }

    int nof_skipped_seq_id = get_nof_skipped_sequence_id(seq_id, expected_seq_id);

    // Update the expected sequence identifier when the sequence identifier is from the future.
    if (nof_skipped_seq_id > 0) {
      counters[eaxc] = seq_id;
    }

    return nof_skipped_seq_id;
  }

private:
  /// \brief Returns the number of skipped sequence identifiers using the given sequence identifier and expected
  /// sequence identifier.
  ///
  /// A negative difference means that the sequence identifier received is from the past.
  /// No difference means that the sequence identifier matches the expected.
  /// A positive difference means that the sequence identifier is from the future.
  static int get_nof_skipped_sequence_id(uint8_t seq_id, uint8_t expected_seq_id)
  {
    int a = static_cast<int>(seq_id) - static_cast<int>(expected_seq_id);
    if (a >= HALF_NOF_SEQUENCES_IDENTIFIERS) {
      return a - NOF_SEQUENCES_IDENTIFIERS;
    }
    if (a < -HALF_NOF_SEQUENCES_IDENTIFIERS) {
      return a + NOF_SEQUENCES_IDENTIFIERS;
    }
    return a;
  }
};

} // namespace ofh
} // namespace srsran
