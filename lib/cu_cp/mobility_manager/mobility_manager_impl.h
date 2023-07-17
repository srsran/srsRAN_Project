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

#include "srsran/cu_cp/mobility_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Basic cell manager implementation
class mobility_manager_impl final : public mobility_manager
{
public:
  mobility_manager_impl(const mobility_manager_cfg& cfg, du_repository& du_db_, du_processor_ue_manager& ue_mng_);
  ~mobility_manager_impl() = default;

  void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) override;

private:
  void handle_inter_cu_handover(ue_index_t source_ue_index, pci_t neighbor_pci);
  void handle_inter_du_handover(ue_index_t source_ue_index,
                                pci_t      neighbor_pci,
                                du_index_t source_du_index,
                                du_index_t target_du_index);
  void handle_intra_du_handover(ue_index_t source_ue_index, pci_t neighbor_pci);

  mobility_manager_cfg cfg;

  du_repository&           du_db;
  du_processor_ue_manager& ue_mng;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
