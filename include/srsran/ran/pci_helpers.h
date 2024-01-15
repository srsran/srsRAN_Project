/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
