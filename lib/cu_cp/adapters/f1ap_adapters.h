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
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_controller;

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
