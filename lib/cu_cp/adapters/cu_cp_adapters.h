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

#include "../cu_cp_impl_interface.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between CU-CP and RRC DU to request UE removal
class cu_cp_rrc_du_adapter : public cu_cp_rrc_ue_notifier, public cu_cp_rrc_du_statistics_notifier
{
public:
  cu_cp_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_ue_handler& rrc_ue_hndlr_, rrc_du_statistics_handler& rrc_du_statistic_handler_)
  {
    rrc_ue_hndlr             = &rrc_ue_hndlr_;
    rrc_du_statistic_handler = &rrc_du_statistic_handler_;
  }

  rrc_ue_interface* find_rrc_ue(ue_index_t ue_index) override
  {
    srsran_assert(rrc_ue_hndlr != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_hndlr->find_ue(ue_index);
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(rrc_ue_hndlr != nullptr, "RRC UE handler must not be nullptr");
    rrc_ue_hndlr->remove_ue(ue_index);
  }

  size_t get_nof_ues() const override
  {
    srsran_assert(rrc_du_statistic_handler != nullptr, "RRC DU statistics handler must not be nullptr");
    return rrc_du_statistic_handler->get_nof_ues();
  }

private:
  rrc_ue_handler*            rrc_ue_hndlr             = nullptr;
  rrc_du_statistics_handler* rrc_du_statistic_handler = nullptr;
};

/// Adapter between CU-CP and RRC UE, to transfer UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_adapter : public cu_cp_rrc_ue_context_transfer_notifier
{
public:
  cu_cp_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_context_handler& rrc_context_handler_) { rrc_context_handler = &rrc_context_handler_; }

  rrc_ue_reestablishment_context_response on_rrc_ue_context_transfer() override
  {
    srsran_assert(rrc_context_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_context_handler->get_context();
  }

private:
  rrc_ue_context_handler* rrc_context_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
