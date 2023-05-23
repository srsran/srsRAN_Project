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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace ofh {

/// Sequence identifier generator.
class sequence_identifier_generator
{
  static_vector<uint8_t, MAX_NOF_SUPPORTED_EAXC> counters = {0, 0, 0, 0};

public:
  /// Generates a new sequence identifier and returns it.
  uint8_t generate(unsigned eaxc)
  {
    srsran_assert(eaxc < MAX_NOF_SUPPORTED_EAXC,
                  "Invalid eAxC={} detected. Maximum eAxC suported={}",
                  eaxc,
                  MAX_NOF_SUPPORTED_EAXC);

    uint8_t& value = counters[eaxc];
    return value++;
  }
};

} // namespace ofh
} // namespace srsran