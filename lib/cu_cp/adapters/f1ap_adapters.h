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

#include "../cu_cp_controller/common_task_scheduler.h"
#include "../du_processor/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_controller;

/// Adapter between F1AP and RRC UE
class f1ap_rrc_ue_adapter : public f1ap_rrc_message_notifier
{
public:
  void connect_rrc_ue(rrc_ul_pdu_handler& rrc_pdu_handler_) { rrc_pdu_handler = &rrc_pdu_handler_; }

  void on_ul_ccch_pdu(byte_buffer pdu) override
  {
    srsran_assert(rrc_pdu_handler != nullptr, "RRC UL handler must not be nullptr");
    rrc_pdu_handler->handle_ul_ccch_pdu(std::move(pdu));
  }

  void on_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) override
  {
    srsran_assert(rrc_pdu_handler != nullptr, "RRC UL handler must not be nullptr");
    rrc_pdu_handler->handle_ul_dcch_pdu(srb_id, std::move(pdu));
  }

private:
  rrc_ul_pdu_handler* rrc_pdu_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
