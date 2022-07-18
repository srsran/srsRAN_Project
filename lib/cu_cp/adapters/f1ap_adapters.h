/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_manager_interfaces.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between F1AP and CU-CP manager, to handle DU specific procedure outcomes (e.g. F1 Remove)
class cu_cp_f1ap_event_indicator : public f1c_du_management_notifier
{
public:
  void connect(cu_cp_du_handler& cu_cp_mng_) { du_handler = &cu_cp_mng_; }
  void on_du_remove_request_received(const du_index_t du_index) override
  {
    srsran_assert(du_handler != nullptr, "DU handler must not be nullptr");
    du_handler->handle_du_remove_request(du_index);
  }

private:
  cu_cp_du_handler* du_handler = nullptr;
};

/// Adapter between F1AP and DU processor
class du_processor_f1ap_event_indicator : public f1c_du_processor_message_notifier, public f1c_rrc_message_notifier
{
public:
  void connect(du_processor_f1c_interface& du_processor_f1c_) { du_f1c_handler = &du_processor_f1c_; }

  du_cell_index_t find_cell(uint64_t packed_nr_cell_id) override
  {
    return du_f1c_handler->find_cell(packed_nr_cell_id);
  }

  void on_f1_setup_request_received(const f1_setup_request_message& msg) override
  {
    srsran_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");
    du_f1c_handler->handle_f1_setup_request(msg);
  }

  ue_index_t on_initial_ul_rrc_message_transfer_received(const du_cell_index_t          pcell_index,
                                                         const f1ap_initial_ul_rrc_msg& msg) override
  {
    srsran_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");

    initial_ul_rrc_message du_proc_msg = {};
    du_proc_msg.pcell_index            = pcell_index;
    du_proc_msg.rrc_container          = msg.msg->rrc_container.value;
    du_proc_msg.c_rnti                 = to_rnti(msg.msg->c_rnti.value);
    du_proc_msg.du_to_cu_rrc_container = msg.msg->duto_currc_container.value;

    if (msg.msg->rrc_container_rrc_setup_complete_present) {
      du_proc_msg.rrc_container_rrc_setup_complete = msg.msg->rrc_container_rrc_setup_complete.value;
    }

    return du_f1c_handler->handle_initial_ul_rrc_message_transfer(du_proc_msg);
  }

  void on_ul_rrc_message_transfer_received(const ue_index_t ue_index, const f1ap_ul_rrc_msg& msg) override
  {
    srsran_assert(du_f1c_handler != nullptr, "F1C handler must not be nullptr");

    ul_rrc_message du_proc_msg = {};
    du_proc_msg.ue_idx         = ue_index;
    du_proc_msg.rrc_container  = msg.msg->rrc_container.value;
    du_proc_msg.srbid          = msg.msg->srbid.value;

    du_f1c_handler->handle_ul_rrc_message_transfer(du_proc_msg);
  }

private:
  du_processor_f1c_interface* du_f1c_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
