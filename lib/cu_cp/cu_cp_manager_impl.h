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
#include "srsgnb/f1_interface/du/f1ap_du.h"
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
  void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) override;
  void handle_ul_rrc_message_transfer(const f1ap_ul_rrc_msg& msg) override;

  size_t get_nof_dus() const override;
  size_t get_nof_ues() const override;

private:
  // F1AP senders

  /// \brief Create and transmit the F1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_f1_setup_response(const du_context& du_ctxt);

  /// \brief Create and transmit the F1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause);

  /// \brief Send RRCSetup message via DL RRC message transfer (TS 38.473 section 8.4.2), as response to an initial UL
  /// RRC message transfer.
  /// \param[in] msg The received initial UL RRC message transfer.
  void send_rrc_setup(const f1ap_initial_ul_rrc_msg& msg);

  // DU manager configuration that will be visible to all running procedures
  cu_cp_manager_config_t cfg;

  // CU-CP context.
  cu_cp_manager_context ctx;

  // Components
  du_manager du_mng;
  ue_manager ue_mng;

  // Handler for DU tasks.
  async_task_sequencer main_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_MANAGER_IMPL_H
