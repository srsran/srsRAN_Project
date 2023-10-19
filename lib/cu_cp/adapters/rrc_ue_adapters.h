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
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP to pass RRC PDUs
class rrc_ue_f1ap_pdu_adapter : public rrc_pdu_f1ap_notifier
{
public:
  explicit rrc_ue_f1ap_pdu_adapter(f1ap_rrc_message_handler& f1ap_handler_, ue_index_t ue_index_) :
    f1ap_handler(f1ap_handler_), ue_index(ue_index_)
  {
  }

  void on_new_rrc_pdu(const srb_id_t srb_id, const byte_buffer& pdu) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = srb_id;
    f1ap_msg.rrc_container       = pdu.copy();
    f1ap_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

private:
  f1ap_rrc_message_handler& f1ap_handler;
  const ue_index_t          ue_index;
};

/// Adapter between RRC UE and DU processor
class rrc_ue_du_processor_adapter : public rrc_ue_du_processor_notifier
{
public:
  void connect_du_processor(du_processor_rrc_ue_interface& du_processor_rrc_ue_)
  {
    du_processor_rrc_ue_handler = &du_processor_rrc_ue_;
  }

  async_task<cu_cp_ue_context_release_complete>
  on_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) override
  {
    srsran_assert(du_processor_rrc_ue_handler != nullptr, "DU processor handler must not be nullptr");
    return du_processor_rrc_ue_handler->handle_ue_context_release_command(cmd);
  }

  async_task<bool> on_rrc_reestablishment_context_modification_required(ue_index_t ue_index) override
  {
    srsran_assert(du_processor_rrc_ue_handler != nullptr, "DU Processor task handler must not be nullptr");
    return du_processor_rrc_ue_handler->handle_rrc_reestablishment_context_modification_required(ue_index);
  }

private:
  du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

/// Adapter between RRC UE and UE Task Scheduler
class rrc_to_du_ue_task_scheduler : public rrc_ue_task_scheduler
{
public:
  rrc_to_du_ue_task_scheduler(ue_index_t ue_index_, task_executor& ue_exec_) : ue_index(ue_index_), ue_exec(&ue_exec_)
  {
  }

  void connect_du_processor(du_processor_ue_task_handler& du_processor_task_handler_)
  {
    du_processor_task_handler = &du_processor_task_handler_;
    timers                    = timer_factory{du_processor_task_handler->get_timer_manager(), *ue_exec};
  }

  void schedule_async_task(async_task<void>&& task) override
  {
    srsran_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    du_processor_task_handler->handle_ue_async_task(ue_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsran_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    return du_processor_task_handler->make_unique_timer();
  }
  timer_factory get_timer_factory() override
  {
    srsran_assert(du_processor_task_handler != nullptr, "DU Processor task handler must not be nullptr");
    return timers;
  }

private:
  ue_index_t                    ue_index;
  task_executor*                ue_exec = nullptr;
  timer_factory                 timers;
  du_processor_ue_task_handler* du_processor_task_handler = nullptr;
};

// Adapter between RRC UE and NGAP
class rrc_ue_ngap_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void connect_ngap(ngap_nas_message_handler&     ngap_nas_msg_handler_,
                    ngap_control_message_handler& ngap_ctrl_msg_handler_)
  {
    ngap_nas_msg_handler  = &ngap_nas_msg_handler_;
    ngap_ctrl_msg_handler = &ngap_ctrl_msg_handler_;
  }

  void on_initial_ue_message(const cu_cp_initial_ue_message& msg) override
  {
    srsran_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");
    ngap_nas_msg_handler->handle_initial_ue_message(msg);
  }

  void on_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) override
  {
    srsran_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");
    ngap_nas_msg_handler->handle_ul_nas_transport_message(msg);
  }

  void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) override
  {
    srsran_assert(ngap_ctrl_msg_handler != nullptr, "NGAP handler must not be nullptr");

    ngap_ctrl_msg_handler->handle_ue_context_release_request(msg);
  }

  void on_inter_cu_ho_rrc_recfg_complete_received(const ue_index_t           ue_index,
                                                  const nr_cell_global_id_t& cgi,
                                                  const unsigned             tac) override
  {
    srsran_assert(ngap_ctrl_msg_handler != nullptr, "NGAP handler must not be nullptr");

    ngap_ctrl_msg_handler->handle_inter_cu_ho_rrc_recfg_complete(ue_index, cgi, tac);
  }

private:
  ngap_nas_message_handler*     ngap_nas_msg_handler  = nullptr;
  ngap_control_message_handler* ngap_ctrl_msg_handler = nullptr;
};

/// Adapter between RRC UE and CU-CP
class rrc_ue_cu_cp_adapter : public rrc_ue_reestablishment_notifier
{
public:
  void connect_cu_cp(cu_cp_rrc_ue_interface& cu_cp_rrc_ue_, cu_cp_ue_removal_handler& ue_removal_handler_)
  {
    cu_cp_rrc_ue_handler = &cu_cp_rrc_ue_;
    ue_removal_handler   = &ue_removal_handler_;
  }

  rrc_reestablishment_ue_context_t
  on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_rrc_reestablishment_request(old_pci, old_c_rnti, ue_index);
  }

  async_task<bool> on_ue_transfer_required(ue_index_t ue_index, ue_index_t old_ue_index) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_ue_context_transfer(ue_index, old_ue_index);
  }

  void on_ue_removal_required(ue_index_t ue_index) override
  {
    srsran_assert(ue_removal_handler != nullptr, "CU-CP UE removal handler must not be nullptr");
    return ue_removal_handler->handle_ue_removal_request(ue_index);
  }

private:
  cu_cp_rrc_ue_interface*   cu_cp_rrc_ue_handler = nullptr;
  cu_cp_ue_removal_handler* ue_removal_handler   = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
