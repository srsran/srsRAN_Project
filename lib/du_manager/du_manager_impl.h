
#ifndef SRSGNB_DU_MANAGER_IMPL_H
#define SRSGNB_DU_MANAGER_IMPL_H

#include "du_manager_context.h"
#include "du_ue_manager.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/task_executor.h"
#include <memory>

namespace srsgnb {

class du_manager_impl final : public du_manager_interface
{
public:
  du_manager_impl(const du_manager_config_t& cfg);

  // RLC interface
  void handle_rlc_ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) override {}
  void handle_rlc_ue_delete_response(const rlc_ue_delete_response_message& resp) override {}

  // MAC interface
  void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) override;

  size_t nof_ues() override;

private:
  // DU manager configuration that will be visible to all running procedures
  du_manager_config_t cfg;

  // Components
  du_ue_manager ue_mng;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_IMPL_H
