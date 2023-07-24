/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/cell_meas_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between cell measurement and mobility manager to trigger handover.
class cell_meas_mobility_manager_adapter : public cell_meas_mobility_manager_notifier
{
public:
  cell_meas_mobility_manager_adapter() = default;

  void connect_mobility_manager(mobility_manager_measurement_handler& handler_) { handler = &handler_; }

  void on_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) override
  {
    srsran_assert(handler != nullptr, "Mobility manager handler must not be nullptr");
    handler->handle_neighbor_better_than_spcell(ue_index, neighbor_pci);
  }

private:
  mobility_manager_measurement_handler* handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
