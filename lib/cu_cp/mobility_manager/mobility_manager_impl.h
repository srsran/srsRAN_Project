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

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/cu_cp/ue_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Handler for measurement related events.
class mobility_manager_measurement_handler
{
public:
  virtual ~mobility_manager_measurement_handler() = default;

  /// \brief Handle event where neighbor became better than serving cell.
  virtual void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

/// Basic cell manager implementation
class mobility_manager final : public mobility_manager_measurement_handler
{
public:
  mobility_manager(const mobility_manager_cfg& cfg, cu_cp_f1c_handler& du_db_, du_processor_ue_manager& ue_mng_);
  ~mobility_manager() = default;

  void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) override;

private:
  void handle_inter_cu_handover(ue_index_t source_ue_index, pci_t neighbor_pci);
  void handle_inter_du_handover(ue_index_t source_ue_index,
                                pci_t      neighbor_pci,
                                du_index_t source_du_index,
                                du_index_t target_du_index);
  void handle_intra_du_handover(ue_index_t source_ue_index, pci_t neighbor_pci);

  mobility_manager_cfg cfg;

  cu_cp_f1c_handler&       du_db;
  du_processor_ue_manager& ue_mng;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
