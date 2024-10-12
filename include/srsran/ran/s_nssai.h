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

#include <optional>

namespace srsran {

/// Network slice information, see TS 38.413, Sec. 9.3.1.24
struct s_nssai_t {
  /// Slice/Service type (max 8bits).
  uint8_t sst = 0;
  /// Slice Differentiator (max 24bits).
  std::optional<uint32_t> sd;

  bool operator==(const s_nssai_t& other) const { return sst == other.sst && sd == other.sd; }
};

} // namespace srsran
