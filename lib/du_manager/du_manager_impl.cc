
#include "du_manager_impl.h"

namespace srsgnb {

du_manager_impl::du_manager_impl(const du_manager_config_t& cfg_) : cfg(cfg_), ue_mng(cfg) {}

void du_manager_impl::ue_create(const du_ue_create_message& msg)
{
  // Switch DU Manager exec context
  cfg.du_mng_exec->execute([this, msg]() {
    // Start UE create procedure
    ue_mng.handle_ue_create_request(msg);
  });
}

std::string du_manager_impl::get_ues()
{
  return ue_mng.get_ues();
}

} // namespace srsgnb
