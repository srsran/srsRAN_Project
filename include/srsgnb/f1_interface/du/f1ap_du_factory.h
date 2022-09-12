#pragma once

#include "f1ap_du.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

/// Creates an instance of an F1AP DU interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1_interface> create_f1ap(f1c_message_notifier&       event_notifier,
                                          f1c_du_configurator&        du_mng,
                                          task_executor&              ctrl_exec,
                                          du_high_ue_executor_mapper& ue_exec_mapper);

} // namespace srs_du
} // namespace srsgnb
