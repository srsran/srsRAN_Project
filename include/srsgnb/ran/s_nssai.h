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

#include "srsgnb/adt/optional.h"

namespace srsran {

// Network slice information, see TS 38.413, Sec. 9.3.1.24
struct s_nssai_t {
  uint8_t            sst; // Slice/Service type (max 8bits)
  optional<uint32_t> sd;  // Slice Differentiator (max 24bits)
};

} // namespace srsran
