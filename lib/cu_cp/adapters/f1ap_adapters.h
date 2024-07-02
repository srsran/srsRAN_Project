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

#include "../cu_cp_controller/common_task_scheduler.h"
#include "../du_processor/du_processor.h"
#include "../du_processor/du_setup_handler.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_controller;

/// Adapter between F1AP and DU processor
class f1ap_du_processor_adapter : public f1ap_du_processor_notifier
{
public:
  f1ap_du_processor_adapter(common_task_scheduler& common_task_sched_, du_setup_handler& du_setup_hdlr_) :
    common_task_sched(&common_task_sched_), du_setup_hdlr(&du_setup_hdlr_)
  {
  }

  void connect_du_processor(du_processor_f1ap_interface& du_processor_f1ap_) { du_f1ap_handler = &du_processor_f1ap_; }

  du_setup_result on_new_du_setup_request(const du_setup_request& msg) override
  {
    srsran_assert(du_setup_hdlr != nullptr, "F1AP handler must not be nullptr");
    return du_setup_hdlr->handle_du_setup_request(msg);
  }

  ue_rrc_context_creation_outcome
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    return du_f1ap_handler->handle_ue_rrc_context_creation_request(req);
  }

  void on_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& req) override
  {
    srsran_assert(du_f1ap_handler != nullptr, "F1AP handler must not be nullptr");
    du_f1ap_handler->handle_du_initiated_ue_context_release_request(req);
  }

  bool schedule_async_task(async_task<void> task) override
  {
    return common_task_sched->schedule_async_task(std::move(task));
  }

  async_task<void> on_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override
  {
    return du_f1ap_handler->handle_ue_transaction_info_loss(ev);
  }

private:
  common_task_scheduler*       common_task_sched = nullptr;
  du_setup_handler*            du_setup_hdlr     = nullptr;
  du_processor_f1ap_interface* du_f1ap_handler   = nullptr;
};

/// Adapter between F1AP and RRC UE
class f1ap_rrc_ue_adapter : public f1ap_rrc_message_notifier
{
public:
  void connect_rrc_ue(rrc_ul_ccch_pdu_handler& rrc_ul_ccch_handler_, rrc_ul_dcch_pdu_handler& rrc_ul_dcch_handler_)
  {
    rrc_ul_ccch_handler = &rrc_ul_ccch_handler_;
    rrc_ul_dcch_handler = &rrc_ul_dcch_handler_;
  }

  void on_ul_ccch_pdu(byte_buffer pdu) override
  {
    srsran_assert(rrc_ul_ccch_handler != nullptr, "RRC UL CCCH handler must not be nullptr");
    rrc_ul_ccch_handler->handle_ul_ccch_pdu(std::move(pdu));
  }

  void on_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) override
  {
    srsran_assert(rrc_ul_dcch_handler != nullptr, "RRC UL DCCH handler must not be nullptr");
    rrc_ul_dcch_handler->handle_ul_dcch_pdu(srb_id, std::move(pdu));
  }

private:
  rrc_ul_ccch_pdu_handler* rrc_ul_ccch_handler = nullptr;
  rrc_ul_dcch_pdu_handler* rrc_ul_dcch_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
