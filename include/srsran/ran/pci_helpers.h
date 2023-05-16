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

#include "pci.h"

namespace srsran {
namespace config_helpers {

/// Returns true if the given struct is valid, otherwise false.
inline bool is_valid(const pci_t pci)
{
  // PCI must be in [0..1007]
  if (pci >= INVALID_PCI) {
    fmt::print("Invalid pci={}.\n", pci);
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
