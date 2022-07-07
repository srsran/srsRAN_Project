/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_F1AP_ADAPTERS_H
#define SRSGNB_CU_CP_F1AP_ADAPTERS_H

#include "../ue_manager_interfaces.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between F1AP and DU processor
/// Adapter between F1AP and UE manager to forward messages to RRC/PDCP
class du_processor_f1ap_event_indicator : public f1c_du_processor_message_notifier
{
public:
  void connect(du_processor_f1c_interface& du_processor_f1c_) { du_f1c_handler = &du_processor_f1c_; }
  void on_f1_setup_request_received(const f1_setup_request_message& msg) override
  {
    srsran_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");
    du_f1c_handler->handle_f1_setup_request(msg);
  }

  void on_initial_ul_rrc_message_transfer_received(const f1ap_initial_ul_rrc_msg& msg) override
  {
    srsran_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");
    du_f1c_handler->handle_initial_ul_rrc_message_transfer(msg);
  }

private:
  du_processor_f1c_interface* du_f1c_handler = nullptr;
};

class ue_manager_f1ap_event_indicator : public f1c_ue_manager_message_notifier
{
public:
  void connect(ue_manager_rrc_message_handler& ue_manager_f1c_handler_)
  {
    ue_manager_f1c_handler = &ue_manager_f1c_handler_;
  }

  void on_ul_rrc_message_transfer_received(const f1ap_ul_rrc_msg& msg) override
  {
    srsran_assert(ue_manager_f1c_handler != nullptr, "F1C handler must not be nullptr");

    ue_manager_f1c_handler->handle_ul_rrc_message_transfer(msg);
  }

private:
  ue_manager_rrc_message_handler* ue_manager_f1c_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_CU_CP_F1AP_ADAPTERS_H
