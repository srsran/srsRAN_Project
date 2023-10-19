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

#include "../cu_cp_impl_interface.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between CU-CP and NGAP, to initialize connection procedures
class cu_cp_ngap_adapter : public cu_cp_ngap_control_notifier, public cu_cp_ngap_statistics_notifier
{
public:
  void connect_ngap(ngap_connection_manager&         ngap_conn_mng_,
                    ngap_control_message_handler&    ngap_ctrl_handler_,
                    ngap_ue_context_removal_handler& ngap_ue_handler_,
                    ngap_statistics_handler&         ngap_statistic_handler_)
  {
    ngap_conn_mng          = &ngap_conn_mng_;
    ngap_ctrl_handler      = &ngap_ctrl_handler_;
    ngap_ue_handler        = &ngap_ue_handler_;
    ngap_statistic_handler = &ngap_statistic_handler_;
  }

  async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) override
  {
    srsran_assert(ngap_conn_mng != nullptr, "NGAP handler must not be nullptr");
    return ngap_conn_mng->handle_ng_setup_request(request);
  }

  void on_ue_context_release_request(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(ngap_ctrl_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_ctrl_handler->handle_ue_context_release_request(request);
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(ngap_ue_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_ue_handler->remove_ue_context(ue_index);
  }

  size_t get_nof_ues() const override
  {
    srsran_assert(ngap_statistic_handler != nullptr, "NGAP statistics handler must not be nullptr");
    return ngap_statistic_handler->get_nof_ues();
  }

private:
  ngap_connection_manager*         ngap_conn_mng          = nullptr;
  ngap_control_message_handler*    ngap_ctrl_handler      = nullptr;
  ngap_ue_context_removal_handler* ngap_ue_handler        = nullptr;
  ngap_statistics_handler*         ngap_statistic_handler = nullptr;
};

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
class cu_cp_rrc_du_adapter : public cu_cp_rrc_ue_removal_notifier, public cu_cp_rrc_du_statistics_notifier
{
public:
  cu_cp_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_ue_removal_handler& ue_removal_handler_, rrc_du_statistics_handler& rrc_du_statistic_handler_)
  {
    ue_removal_handler       = &ue_removal_handler_;
    rrc_du_statistic_handler = &rrc_du_statistic_handler_;
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(ue_removal_handler != nullptr, "RRC UE removal handler must not be nullptr");
    ue_removal_handler->remove_ue(ue_index);
  }

  size_t get_nof_ues() const override
  {
    srsran_assert(rrc_du_statistic_handler != nullptr, "RRC DU statistics handler must not be nullptr");
    return rrc_du_statistic_handler->get_nof_ues();
  }

private:
  rrc_ue_removal_handler*    ue_removal_handler       = nullptr;
  rrc_du_statistics_handler* rrc_du_statistic_handler = nullptr;
};

/// Adapter between CU-CP and RRC UE, to transfer UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_adapter : public cu_cp_rrc_ue_context_transfer_notifier
{
public:
  cu_cp_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_context_handler& rrc_context_handler_) { rrc_context_handler = &rrc_context_handler_; }

  rrc_reestablishment_ue_context_t on_rrc_ue_context_transfer() override
  {
    srsran_assert(rrc_context_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_context_handler->get_context();
  }

private:
  rrc_ue_context_handler* rrc_context_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
