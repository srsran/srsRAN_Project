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

#include <cstddef>

namespace srsgnb {

/// Maximum number of PDUs per slot. This value is implementation-defined.
constexpr size_t MAX_UL_PDUS_PER_SLOT = 16U;

/// Maximum number of CBs per PDU. This value is implementation-defined.
constexpr size_t MAX_CBS_PER_PDU = 64;

} // namespace srsgnb