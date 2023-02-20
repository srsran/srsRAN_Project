#pragma once

#include "f1ap_du.h"
#include "srsran/du_high/du_high_ue_executor_mapper.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// Creates an instance of an F1AP DU interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_interface> create_f1ap(f1ap_message_notifier&      event_notifier,
                                            f1ap_du_configurator&       du_mng,
                                            task_executor&              ctrl_exec,
                                            du_high_ue_executor_mapper& ue_exec_mapper);

} // namespace srs_du
} // namespace srsran
