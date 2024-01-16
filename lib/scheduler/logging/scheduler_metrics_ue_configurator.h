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

  /// Removes a UE from the reported metrics.
  virtual void handle_ue_deletion(du_ue_index_t ue_index) = 0;
};

} // namespace srsran