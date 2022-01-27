
#include "srsgnb/rlc/rlc_factory.h"
#include "rlc_impl.h"

namespace srsgnb {

std::unique_ptr<rlc_interface> create_rlc(rlc_config_notifier& config_notifier)
{
  std::unique_ptr<rlc_interface> instance = std::make_unique<rlc_impl>(config_notifier);
  return instance;
}

} // namespace srsgnb