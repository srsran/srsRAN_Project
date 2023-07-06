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
  mobility_manager_impl(const mobility_manager_cfg& cfg);
  ~mobility_manager_impl() = default;

  void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) override;

private:
  mobility_manager_cfg cfg;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
