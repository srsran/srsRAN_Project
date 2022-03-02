
#ifndef SRSGNB_DU_UE_MANAGER_H
#define SRSGNB_DU_UE_MANAGER_H

#include "du_manager_config.h"
#include "du_ue_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {

class du_ue_manager : public ue_manager_ctrl_configurer
{
public:
  du_ue_manager(du_manager_config_t& cfg_);

  void create_ue(const du_ue_create_message& msg);

  std::string get_ues();

private:
  struct ue_impl {
    du_ue_context ctxt;

    constexpr static size_t number_of_pending_procedures = 16;
    async_task_sequencer    ctrl_loop{number_of_pending_procedures}; ///< UE Control loop
  };

  ue_impl* add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t pcell_index);

  void remove_ue(du_ue_index_t ue_index) override;

  du_manager_config_t&  cfg;
  srslog::basic_logger& logger;

  slot_array<ue_impl, MAX_NOF_UES> ue_db;

  // UE manager main loop
  async_task_sequencer main_ctrl_loop{64};
};

} // namespace srsgnb

#endif // SRSGNB_DU_UE_MANAGER_H
