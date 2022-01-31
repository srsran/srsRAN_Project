
#ifndef SRSGNB_SCHED_H
#define SRSGNB_SCHED_H

#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

class sched final : public sched_interface
{
public:
  explicit sched(sched_cfg_notifier& notifier) : mac_notifier(notifier) {}

  void config_ue(rnti_t rnti) override { mac_notifier.on_ue_config_complete(rnti); }
  void delete_ue_request(rnti_t rnti) override  { mac_notifier.on_ue_delete_response(rnti); }

private:
  sched_cfg_notifier& mac_notifier;
};

} // namespace srsgnb

#endif // SRSGNB_SCHED_H
