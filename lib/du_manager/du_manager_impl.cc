
#include "du_manager_impl.h"

namespace srsgnb {

du_manager_impl::du_manager_impl(rlc_config_interface&       rlc,
                                 mac_config_interface&       mac,
                                 du_manager_config_notifier& f1ap_notifier)
{
  ctxt.rlc           = &rlc;
  ctxt.mac           = &mac;
  ctxt.f1ap_notifier = &f1ap_notifier;
}

void du_manager_impl::ue_create(const du_ue_create_message& msg)
{
  // Start UE create procedure
  ue_create_proc = std::make_unique<ue_creation_procedure>(ctxt, msg);
}

void du_manager_impl::rlc_ue_create_response(const rlc_ue_create_response_message& resp)
{
  ue_create_proc->rlc_ue_create_response(resp);
}

void du_manager_impl::mac_ue_create_response(const mac_ue_create_request_response_message& resp)
{
  ue_create_proc->mac_ue_create_response(resp);

  // Procedure complete
  ue_create_proc.reset();
}

} // namespace srsgnb