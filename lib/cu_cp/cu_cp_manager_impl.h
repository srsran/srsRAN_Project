/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_MANAGER_IMPL_H
#define SRSGNB_CU_CP_MANAGER_IMPL_H

#include "cu_cp_manager_context.h"
#include "du_manager.h"
#include "srsgnb/cu_cp/cu_cp_manager.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/support/executors/task_executor.h"
#include "ue_manager.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class cu_cp_manager_impl final : public cu_cp_manager_interface
{
public:
  cu_cp_manager_impl(const cu_cp_manager_config_t& cfg);

  // cu_cp_manager_f1c_interface
  void handle_f1_setup_request(const f1_setup_request_message& msg) override;
  void handle_ul_rrc_message_transfer(const ul_rrc_message_transfer_message& msg) override;

  size_t nof_ues() override;

private:
  // DU manager configuration that will be visible to all running procedures
  cu_cp_manager_config_t cfg;

  // CU-CP context.
  cu_cp_manager_context ctx;

  // Components
  ue_manager ue_mng;
  du_manager du_mng;

  // Handler for DU tasks.
  async_task_sequencer main_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_IMPL_H
