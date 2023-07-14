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

#include "../cu_cp_impl_interface.h"
#include "srsran/cu_cp/mobility_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between cell measurement and mobility manager to trigger handover.
class mobility_manager_cu_cp_adapter : public mobility_manager_cu_cp_notifier
{
public:
  mobility_manager_cu_cp_adapter() = default;

  void connect_cu_cp(cu_cp_mobility_manager_handler& handler_) { handler = &handler_; }

  void on_inter_du_handover_request(ue_index_t ue_index, pci_t target_pci) override
  {
    srsran_assert(handler != nullptr, "CU-CP handler must not be nullptr");
    handler->handle_inter_du_handover_request(ue_index, target_pci);
  }

private:
  cu_cp_mobility_manager_handler* handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
