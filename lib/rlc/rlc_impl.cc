
#include "rlc_impl.h"

namespace srsgnb {

rlc_impl::rlc_impl(rlc_config_notifier& config_notifier_) : config_notifier(config_notifier_) {}

void rlc_impl::ue_create(const rlc_ue_create_message& msg)
{
  // TODO: Create UE

  rlc_ue_create_response_message resp{};
  resp.cell_index = msg.cell_index;
  resp.ue_index   = msg.ue_index;
  resp.result     = true;
  config_notifier.ue_create_response(resp);
}

void rlc_impl::start_ue_reconfiguration(const rlc_ue_reconfiguration_message& msg)
{
  rlc_ue_reconfiguration_response_message resp{};
  resp.cell_index = msg.cell_index;
  resp.ue_index   = msg.ue_index;
  resp.result     = true;
  config_notifier.ue_reconfiguration_response(resp);
}

} // namespace srsgnb