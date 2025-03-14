/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "../ue_manager/cu_cp_ue_impl_interface.h"
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/nrppa/nrppa.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between NRPPa and CU-CP.
class nrppa_cu_cp_adapter : public nrppa_cu_cp_notifier
{
public:
  nrppa_cu_cp_adapter() = default;

  void connect_cu_cp(cu_cp_nrppa_handler& cu_cp_handler_) { cu_cp_handler = &cu_cp_handler_; }

  nrppa_cu_cp_ue_notifier* on_new_nrppa_ue(ue_index_t ue_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NRPPA handler must not be nullptr");
    return cu_cp_handler->handle_new_nrppa_ue(ue_index);
  }

  void on_ul_nrppa_pdu(const byte_buffer& nrppa_pdu, std::variant<ue_index_t, amf_index_t> ue_or_amf_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NRPPA handler must not be nullptr");
    cu_cp_handler->handle_ul_nrppa_pdu(nrppa_pdu, ue_or_amf_index);
  }

  async_task<trp_information_cu_cp_response_t>
  on_trp_information_request(const trp_information_request_t& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NRPPA handler must not be nullptr");
    return cu_cp_handler->handle_trp_information_request(request);
  }

private:
  cu_cp_nrppa_handler* cu_cp_handler = nullptr;
};

/// Adapter between NRPPA and CU-CP UE.
class nrppa_cu_cp_ue_adapter : public nrppa_cu_cp_ue_notifier
{
public:
  nrppa_cu_cp_ue_adapter() = default;

  void connect_ue(cu_cp_ue_impl_interface& ue_) { ue = &ue_; }

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() const override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_ue_index();
  }

  /// \brief Get the index of the DU where the UE is connected.
  du_index_t get_du_index() const override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_du_index();
  }

  std::optional<cell_measurement_positioning_info>& on_measurement_results_required() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_measurement_results();
  }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().schedule_async_task(std::move(task));
  }

private:
  cu_cp_ue_impl_interface* ue = nullptr;
};

/// Adapter between NRPPa and F1AP.
class nrppa_f1ap_adapter : public nrppa_f1ap_notifier
{
public:
  nrppa_f1ap_adapter() = default;

  void connect_f1ap(f1ap_nrppa_message_handler& f1ap_handler_) { f1ap_handler = &f1ap_handler_; }

  async_task<expected<positioning_information_response_t, positioning_information_failure_t>>
  on_positioning_information_request(const positioning_information_request_t& request) override
  {
    srsran_assert(f1ap_handler != nullptr, "F1AP NRPPA handler must not be nullptr");
    return f1ap_handler->handle_positioning_information_request(request);
  }

  async_task<expected<positioning_activation_response_t, positioning_activation_failure_t>>
  on_positioning_activation_request(const positioning_activation_request_t& request) override
  {
    srsran_assert(f1ap_handler != nullptr, "F1AP NRPPA handler must not be nullptr");
    return f1ap_handler->handle_positioning_activation_request(request);
  }

  async_task<expected<measurement_response_t, measurement_failure_t>>
  on_measurement_information_request(const measurement_request_t& request) override
  {
    srsran_assert(f1ap_handler != nullptr, "F1AP NRPPA handler must not be nullptr");
    return f1ap_handler->handle_measurement_information_request(request);
  }

private:
  f1ap_nrppa_message_handler* f1ap_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
