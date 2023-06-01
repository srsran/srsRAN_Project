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

#include "srsran/adt/circular_map.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace ofh {

/// Sequence identifier generator.
class sequence_identifier_generator
{
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> counters;

public:
  /// Default constructor.
  sequence_identifier_generator()
  {
    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      counters.insert(K, 0);
    }
  }

  /// Generates a new sequence identifier and returns it.
  uint8_t generate(unsigned eaxc)
  {
    srsran_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE,
                  "Invalid eAxC={} detected. Maximum supported eAxC value = {}",
                  eaxc,
                  MAX_SUPPORTED_EAXC_ID_VALUE);

    uint8_t& value = counters[eaxc];
    return value++;
  }
};

} // namespace ofh
} // namespace srsran