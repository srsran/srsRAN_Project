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

#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {

namespace srs_cu_cp {

/// Handler for measurement related events.
class cu_cp_mobility_manager_ho_trigger_handler
{
public:
  virtual ~cu_cp_mobility_manager_ho_trigger_handler() = default;

  /// \brief Trigger handover to a target cell.
  ///
  /// The UE is uniquely identified in the CU-CP through the serving Cell PCI
  /// and RNTI. The target is identified through the Target PCI.
  virtual void trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
