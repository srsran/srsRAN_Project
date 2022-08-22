/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace srsgnb {

/// PDCP NR sequence number field
enum class pdcp_sn_size : uint16_t { size12bits = 12, size18bits = 18 };
constexpr uint16_t to_number(pdcp_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

///
/// \brief Configurable parameters for PDCP
///
/// Ref: 3GPP TS TODO
///
struct pdcp_config {
  struct {
    pdcp_sn_size sn_size;
  } tx;
  struct {
    pdcp_sn_size sn_size;
  } rx;
};

} // namespace srsgnb
