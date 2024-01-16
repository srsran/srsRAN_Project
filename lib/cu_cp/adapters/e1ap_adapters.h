/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/cu_cp/cu_up_processor.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between E1AP and CU-UP processor
class e1ap_cu_up_processor_adapter : public e1ap_cu_up_processor_notifier
{
public:
  void connect_cu_up_processor(cu_up_processor_e1ap_interface& cu_up_processor_e1ap_)
  {
    cu_up_e1ap_handler = &cu_up_processor_e1ap_;
  }

  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    srsran_assert(cu_up_e1ap_handler != nullptr, "E1AP handler must not be nullptr");
    cu_up_e1ap_handler->handle_cu_up_e1_setup_request(msg);
  }

private:
  cu_up_processor_e1ap_interface* cu_up_e1ap_handler = nullptr;
};

/// Adapter between E1AP and CU-CP
class e1ap_cu_cp_adapter : public e1ap_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_e1ap_handler& cu_cp_handler_) { cu_cp_handler = &cu_cp_handler_; }

  /// \brief Notifies about the creation of an E1AP.
  /// \param[in] bearer_context_manager The E1AP Bearer Context Manager interface.
  /// \param[in] bearer_removal_handler The E1AP bearer context removal handler.
  /// \param[in] e1ap_statistic_handler The E1AP statistic interface.
  void on_e1ap_created(e1ap_bearer_context_manager&         bearer_context_manager,
                       e1ap_bearer_context_removal_handler& bearer_removal_handler,
                       e1ap_statistics_handler&             e1ap_statistic_handler) override
  {
    srsran_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    cu_cp_handler->handle_e1ap_created(bearer_context_manager, bearer_removal_handler, e1ap_statistic_handler);
  }

  void on_bearer_context_inactivity_notification_received(const cu_cp_inactivity_notification& msg) override
  {
    srsran_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    cu_cp_handler->handle_bearer_context_inactivity_notification(msg);
  }

private:
  cu_cp_e1ap_handler* cu_cp_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
