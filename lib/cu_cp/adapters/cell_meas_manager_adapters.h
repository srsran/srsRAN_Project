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

#include "../cell_meas_manager/cell_meas_manager_impl.h"
#include "../mobility_manager/mobility_manager_impl.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between cell measurement and mobility manager to trigger handover.
class cell_meas_mobility_manager_adapter : public cell_meas_mobility_manager_notifier
{
public:
  cell_meas_mobility_manager_adapter() = default;

  void connect_mobility_manager(mobility_manager_measurement_handler& handler_) { handler = &handler_; }

  void on_neighbor_better_than_spcell(ue_index_t       ue_index,
                                      gnb_id_t         neighbor_gnb_id,
                                      nr_cell_identity neighbor_nci,
                                      pci_t            neighbor_pci) override
  {
    srsran_assert(handler != nullptr, "Mobility manager handler must not be nullptr");
    handler->handle_neighbor_better_than_spcell(ue_index, neighbor_gnb_id, neighbor_nci, neighbor_pci);
  }

private:
  mobility_manager_measurement_handler* handler = nullptr;
};

/// Adapter between cell measurement manager and CU-CP.
class cell_meas_manager_cu_cp_adapter : public cell_meas_manager_cu_cp_notifier
{
public:
  cell_meas_manager_cu_cp_adapter() = default;

  void connect_cu_cp(cu_cp_positioning_measurement_handler& handler_) { handler = &handler_; }

  void on_valid_ue_measurement(ue_index_t ue_index, const cell_measurement_positioning_info& meas_result) override
  {
    srsran_assert(handler != nullptr, "Positioning measurement handler must not be nullptr");
    handler->handle_valid_ue_measurement(ue_index, meas_result);
  }

private:
  cu_cp_positioning_measurement_handler* handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
