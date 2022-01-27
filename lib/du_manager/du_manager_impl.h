
#ifndef SRSGNB_DU_MANAGER_IMPL_H
#define SRSGNB_DU_MANAGER_IMPL_H

#include "du_manager_context.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "ue_creation_procedure.h"
#include <memory>

namespace srsgnb {

class du_manager_impl final : public du_manager_interface
{
public:
  du_manager_impl(rlc_config_interface& rlc_, mac_config_interface& mac_, du_manager_config_notifier& f1ap_notifier);

  // F1AP interface
  void ue_create(const du_ue_create_message& msg) override;

  // RLC interface
  void rlc_ue_create_response(const rlc_ue_create_response_message& resp) override;
  void rlc_ue_reconfiguration_response(const rlc_ue_reconfiguration_response_message& resp) override {}
  void rlc_ue_delete_response(const rlc_ue_delete_response_message& resp) override {}

  // MAC interface
  void mac_ue_create_response(const mac_ue_create_request_response_message& resp) override;
  void mac_ue_reconfiguration_response() override {}

private:
  // DU manager context that will be visible to running procedures
  du_manager_context ctxt;

  // Procedures
  std::unique_ptr<ue_creation_procedure> ue_create_proc;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_IMPL_H
