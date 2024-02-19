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
#include "../du_processor/du_processor_impl_interface.h"
#include "../ue_manager/ue_task_scheduler.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between NGAP and Task Scheduler
class ngap_to_cu_cp_task_scheduler : public ngap_ue_task_scheduler
{
public:
  ngap_to_cu_cp_task_scheduler() = default;

  void connect_cu_cp(ue_task_scheduler_manager& cu_cp_task_sched_) { cu_cp_task_sched = &cu_cp_task_sched_; }

  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    logger.debug("ue={}: Scheduling async task", ue_index);
    cu_cp_task_sched->handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsran_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->get_timer_manager();
  }

private:
  ue_task_scheduler_manager* cu_cp_task_sched = nullptr;
  srslog::basic_logger&      logger           = srslog::fetch_basic_logger("NGAP");
};

/// Adapter between NGAP and CU-CP
class ngap_cu_cp_adapter : public ngap_cu_cp_du_repository_notifier, public ngap_cu_cp_ue_creation_notifier
{
public:
  explicit ngap_cu_cp_adapter() = default;

  void connect_cu_cp(du_repository_ngap_handler&     du_repository_handler_,
                     cu_cp_ngap_ue_creation_handler& ngap_ue_creation_handler_)
  {
    du_repository_handler    = &du_repository_handler_;
    ngap_ue_creation_handler = &ngap_ue_creation_handler_;
  }

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    srsran_assert(du_repository_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    du_repository_handler->handle_paging_message(msg);
  }

  ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi) override
  {
    srsran_assert(du_repository_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    return du_repository_handler->handle_ue_index_allocation_request(cgi);
  }

  async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    srsran_assert(du_repository_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    return du_repository_handler->handle_ngap_handover_request(request);
  }

  bool on_new_ngap_ue(ue_index_t ue_index) override
  {
    srsran_assert(ngap_ue_creation_handler != nullptr, "CU-CP NGAP UE creation handler must not be nullptr");
    return ngap_ue_creation_handler->handle_new_ngap_ue(ue_index);
  }

private:
  du_repository_ngap_handler*     du_repository_handler    = nullptr;
  cu_cp_ngap_ue_creation_handler* ngap_ue_creation_handler = nullptr;
};

/// Adapter between NGAP and RRC UE
class ngap_rrc_ue_adapter : public ngap_rrc_ue_pdu_notifier, public ngap_rrc_ue_control_notifier
{
public:
  ngap_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_dl_nas_message_handler*           rrc_ue_msg_handler_,
                      rrc_ue_init_security_context_handler* rrc_ue_security_handler_,
                      rrc_ue_handover_preparation_handler*  rrc_ue_ho_prep_handler_)
  {
    rrc_ue_msg_handler      = rrc_ue_msg_handler_;
    rrc_ue_security_handler = rrc_ue_security_handler_;
    rrc_ue_ho_prep_handler  = rrc_ue_ho_prep_handler_;
  }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    srsran_assert(rrc_ue_msg_handler != nullptr, "RRC UE message handler must not be nullptr");
    rrc_ue_msg_handler->handle_dl_nas_transport_message(std::move(nas_pdu));
  }

  async_task<bool> on_new_security_context(const security::security_context& sec_context) override
  {
    srsran_assert(rrc_ue_security_handler != nullptr, "RRC UE security handler must not be nullptr");
    return rrc_ue_security_handler->handle_init_security_context(sec_context);
  }

  bool on_security_enabled() override
  {
    srsran_assert(rrc_ue_security_handler != nullptr, "RRC UE security handler must not be nullptr");
    return rrc_ue_security_handler->get_security_enabled();
  }

  byte_buffer on_handover_preparation_message_required() override
  {
    srsran_assert(rrc_ue_ho_prep_handler != nullptr, "RRC UE up manager must not be nullptr");
    return rrc_ue_ho_prep_handler->get_packed_handover_preparation_message();
  }

  bool on_new_rrc_handover_command(byte_buffer cmd) override
  {
    srsran_assert(rrc_ue_ho_prep_handler != nullptr, "RRC UE up manager must not be nullptr");
    return rrc_ue_ho_prep_handler->handle_rrc_handover_command(std::move(cmd));
  }

private:
  rrc_dl_nas_message_handler*           rrc_ue_msg_handler      = nullptr;
  rrc_ue_init_security_context_handler* rrc_ue_security_handler = nullptr;
  rrc_ue_handover_preparation_handler*  rrc_ue_ho_prep_handler  = nullptr;
};

/// Adapter between NGAP and DU Processor
class ngap_du_processor_adapter : public ngap_du_processor_control_notifier
{
public:
  ngap_du_processor_adapter() = default;

  void connect_du_processor(du_processor_ngap_interface* du_processor_ngap_handler_)
  {
    du_processor_ngap_handler = du_processor_ngap_handler_;
  }

  async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override
  {
    srsran_assert(du_processor_ngap_handler != nullptr, "DU Processor handler must not be nullptr");
    return du_processor_ngap_handler->handle_new_pdu_session_resource_setup_request(request);
  }

  async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) override
  {
    srsran_assert(du_processor_ngap_handler != nullptr, "DU Processor handler must not be nullptr");
    return du_processor_ngap_handler->handle_new_pdu_session_resource_modify_request(request);
  }

  async_task<cu_cp_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(cu_cp_pdu_session_resource_release_command& command) override
  {
    srsran_assert(du_processor_ngap_handler != nullptr, "DU Processor handler must not be nullptr");
    return du_processor_ngap_handler->handle_new_pdu_session_resource_release_command(command);
  }

  async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) override
  {
    srsran_assert(du_processor_ngap_handler != nullptr, "DU Processor handler must not be nullptr");
    return du_processor_ngap_handler->handle_ue_context_release_command(command);
  }

private:
  du_processor_ngap_interface* du_processor_ngap_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
