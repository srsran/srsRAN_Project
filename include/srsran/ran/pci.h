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

#include <cstdint>

namespace srsran {

/// 3GPP TS 38.331, PhysCellId ::= INTEGER (0..1007)
using pci_t             = uint16_t;
constexpr pci_t MAX_PCI = 1007;

} // namespace srsran
