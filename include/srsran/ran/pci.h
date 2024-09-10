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

/// 3GPP TS 38.331, PhysCellId ::= INTEGER (0..1007).
using pci_t                 = uint16_t;
constexpr pci_t MIN_PCI     = 0;
constexpr pci_t MAX_PCI     = 1007;
constexpr pci_t INVALID_PCI = MAX_PCI + 1;

/// Returns true if the given struct is valid, otherwise false.
constexpr bool is_valid(pci_t pci)
{
  // PCI must be in [0..1007].
  return pci < INVALID_PCI;
}

} // namespace srsran
