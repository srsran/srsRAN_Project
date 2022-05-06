
#ifndef SRSGNB_MAC_SCHEDULER_H
#define SRSGNB_MAC_SCHEDULER_H

#include "scheduler_configurator.h"
#include "scheduler_feedback_handler.h"
#include "scheduler_slot_handler.h"

namespace srsgnb {

class mac_scheduler : public scheduler_configurator, public scheduler_feedback_handler, public scheduler_slot_handler
{
public:
  virtual ~mac_scheduler() = default;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_SCHEDULER_H
