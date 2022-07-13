/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_MANAGER_H
#define SRSGNB_CU_CP_UE_MANAGER_H

#include "cu_cp_manager_config.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "ue_manager_interfaces.h"

namespace srsgnb {

namespace srs_cu_cp {

class ue_manager : public ue_manager_ctrl_configurer, public ue_manager_f1c_handler
{
public:
  explicit ue_manager(cu_cp_manager_config_t& cfg_);

  const slot_array<ue_context, MAX_NOF_UES>& get_ues() const { return ue_db; }

  ue_context* add_ue(ue_context u) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;
  ue_context* find_rnti(rnti_t rnti) override;
  size_t      get_nof_ues() override;

  ue_index_t get_next_ue_index() override;

  void handle_initial_ul_rrc_message_transfer(const ue_manager_initial_ul_rrc_message& msg) override;
  void handle_ul_rrc_message_transfer(const ue_manager_ul_rrc_message& msg) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<ue_context, MAX_NOF_UES> ue_db;
  std::array<int, MAX_NOF_UES>        rnti_to_ue_index;

  // task event loops indexed by ue_index
  slot_array<async_task_sequencer, MAX_NOF_UES> ue_ctrl_loop;

  ue_manager_f1ap_event_indicator f1ap_ev_notifier;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_MANAGER_H
