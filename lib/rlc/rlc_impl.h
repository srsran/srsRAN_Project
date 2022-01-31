
#ifndef SRSGNB_RLC_IMPL_H
#define SRSGNB_RLC_IMPL_H

#include "srsgnb/rlc/rlc.h"
#include "srsran/srslog/srslog.h"

namespace srsgnb {

class rlc_impl : public rlc_interface
{
public:
  explicit rlc_impl(rlc_config_notifier& config_notifier_);

  void ue_create(const rlc_ue_create_message& cfg) override;
  void start_ue_reconfiguration(const rlc_ue_reconfiguration_message& cfg) override;
  void start_ue_delete(const rlc_ue_delete_message& cfg) override {}
  void ue_reestablishment_request(const rlc_ue_reestablishment_message& cfg) override {}

private:
  rlc_config_notifier& config_notifier;
  srslog::basic_logger& logger;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
