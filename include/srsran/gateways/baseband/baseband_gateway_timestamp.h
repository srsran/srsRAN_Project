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

/// Describes a baseband gateway timestamp. It is expressed in multiples of the sample time.
using baseband_gateway_timestamp = uint64_t;

} // namespace srsran
