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

#include "du_manager_config.h"
#include "du_manager_interfaces.h"
#include "du_ue_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_du {

class du_ue_manager : public ue_manager_ctrl_configurer
{
public:
  explicit du_ue_manager(du_manager_config_t& cfg_);

  void handle_ue_create_request(const ul_ccch_indication_message& msg);
  void handle_ue_delete_request(const du_ue_delete_message& msg);

  const slot_array<du_ue_context, MAX_NOF_DU_UES>& get_ues() { return ue_db; }

private:
  du_ue_context* add_ue(du_ue_context ue_ctx) override;
  du_ue_context* find_ue(du_ue_index_t ue_index) override;
  du_ue_context* find_rnti(rnti_t rnti) override;
  void           remove_ue(du_ue_index_t ue_index) override;

  du_manager_config_t&  cfg;
  srslog::basic_logger& logger;

  slot_array<du_ue_context, MAX_NOF_DU_UES> ue_db;
  std::array<int, MAX_NOF_DU_UES>           rnti_to_ue_index;

  // task event loops indexed by ue_index
  slot_array<async_task_sequencer, MAX_NOF_DU_UES> ue_ctrl_loop;
};

} // namespace srs_du
} // namespace srsgnb
