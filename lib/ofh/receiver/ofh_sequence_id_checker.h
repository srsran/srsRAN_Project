/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/circular_map.h"
#include "srsran/ofh/ofh_constants.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sequence identifier checker.
class sequence_id_checker
{
  static constexpr int NOF_SEQUENCES_IDENTIFIERS      = 1u << 8;
  static constexpr int HALF_NOF_SEQUENCES_IDENTIFIERS = NOF_SEQUENCES_IDENTIFIERS / 2;

  bounded_bitset<MAX_SUPPORTED_EAXC_ID_VALUE>                  initialized;
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> counters;

public:
  /// Default constructor.
  sequence_id_checker() : initialized(MAX_SUPPORTED_EAXC_ID_VALUE)
  {
    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      counters.insert(K, 0);
    }
  }

  /// \brief Updates the expected sequence identifier value for the given eAxC and compares it with the given sequence
  /// identifier, returning the difference between them.
  ///
  /// A negative difference means that the sequence identifier is from the past.
  /// A difference of 0 means that the current sequence identifier matches the expected.
  /// A positive difference means that the sequence identifier belongs to the future. In this case, the expected
  /// sequence identifier is updated with the given sequence identifier.
  int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id)
  {
    srsran_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE,
                  "Invalid eAxC={} detected. Maximum supported eAxC value = {}",
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
