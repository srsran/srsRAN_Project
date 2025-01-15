/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// Adds/Removes UEs from the metrics.
class sched_metrics_ue_configurator
{
public:
  virtual ~sched_metrics_ue_configurator() = default;

  /// Adds a new UE to the reported metrics.
  virtual void handle_ue_creation(du_ue_index_t ue_index, rnti_t rnti, pci_t pcell_pci) = 0;

  /// Handle a reconfiguration of an existing UE.
  virtual void handle_ue_reconfiguration(du_ue_index_t ue_index) = 0;

  /// Removes a UE from the reported metrics.
  virtual void handle_ue_deletion(du_ue_index_t ue_index) = 0;
};

} // namespace srsran
