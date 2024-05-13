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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_mobility_command_handler
{
public:
  virtual ~cu_cp_mobility_command_handler() = default;

  /// \brief Trigger handover of a given UE to a target cell.
  ///
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI
  /// and RNTI. The target is identified through the Target PCI.
  virtual void trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci) = 0;
};

/// Handler for external commands to the CU-CP.
class cu_cp_command_handler
{
public:
  virtual ~cu_cp_command_handler() = default;

  /// Get handler for mobility commands.
  virtual cu_cp_mobility_command_handler& get_mobility_command_handler() = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
