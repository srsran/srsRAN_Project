
#ifndef SRSGNB_DU_UE_MANAGER_H
#define SRSGNB_DU_UE_MANAGER_H

#include "du_manager_config.h"
#include "du_manager_interfaces.h"
#include "du_ue_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {

class du_ue_manager : public ue_manager_ctrl_configurer
{
public:
  explicit du_ue_manager(du_manager_config_t& cfg_);

  void handle_ue_create_request(const du_ue_create_message& msg);

  std::string get_ues();

private:
  du_ue_context* add_ue(du_ue_context ue_ctx) override;
  du_ue_context* find_ue(du_ue_index_t ue_index) override;
  void           remove_ue(du_ue_index_t ue_index) override;

  du_manager_config_t&  cfg;
  srslog::basic_logger& logger;

  slot_array<du_ue_context, MAX_NOF_UES> ue_db;

  // task event loops indexed by ue_index
  slot_array<async_task_sequencer, MAX_NOF_UES> ue_ctrl_loop;
};

} // namespace srsgnb

#endif // SRSGNB_DU_UE_MANAGER_H
