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

/// Adapter between CU-CP and E1AP to request UE removal
class cu_cp_e1ap_adapter : public cu_cp_e1ap_ue_removal_notifier, public cu_cp_e1ap_statistics_notifier
{
public:
  cu_cp_e1ap_adapter() = default;

  void connect_e1ap(e1ap_bearer_context_removal_handler& bearer_context_removal_,
                    e1ap_statistics_handler&             e1ap_statistic_handler_)
  {
    bearer_context_removal = &bearer_context_removal_;
    e1ap_statistic_handler = &e1ap_statistic_handler_;
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(bearer_context_removal != nullptr, "E1AP bearer context removal handler must not be nullptr");
    bearer_context_removal->remove_bearer_context(ue_index);
  }

  size_t get_nof_ues() const override
  {
    srsran_assert(e1ap_statistic_handler != nullptr, "E1AP statistics handler must not be nullptr");
    return e1ap_statistic_handler->get_nof_ues();
  }

private:
  e1ap_bearer_context_removal_handler* bearer_context_removal = nullptr;
  e1ap_statistics_handler*             e1ap_statistic_handler = nullptr;
};

/// Adapter between CU-CP and F1AP to request UE removal
class cu_cp_f1ap_adapter : public cu_cp_f1ap_ue_removal_notifier, public cu_cp_e1ap_statistics_notifier
{
public:
  cu_cp_f1ap_adapter() = default;

  void connect_f1ap(f1ap_ue_context_removal_handler& f1ap_ue_handler_, f1ap_statistics_handler& f1ap_statistic_handler_)
  {
    f1ap_ue_handler        = &f1ap_ue_handler_;
    f1ap_statistic_handler = &f1ap_statistic_handler_;
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(f1ap_ue_handler != nullptr, "F1AP handler must not be nullptr");
    f1ap_ue_handler->remove_ue_context(ue_index);
  }

  size_t get_nof_ues() const override
  {
    srsran_assert(f1ap_statistic_handler != nullptr, "F1AP statistics handler must not be nullptr");
    return f1ap_statistic_handler->get_nof_ues();
  }

private:
  f1ap_ue_context_removal_handler* f1ap_ue_handler        = nullptr;
  f1ap_statistics_handler*         f1ap_statistic_handler = nullptr;
};

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
