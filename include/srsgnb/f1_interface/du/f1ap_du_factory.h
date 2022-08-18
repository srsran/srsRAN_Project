#pragma once

#include "f1ap_du.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

/// Creates an instance of an F1AP DU interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1_interface> create_f1ap(f1c_message_notifier& event_notifier, f1c_task_scheduler& task_sched);

} // namespace srs_du
} // namespace srsgnb
