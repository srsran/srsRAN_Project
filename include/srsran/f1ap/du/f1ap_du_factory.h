#pragma once

#include "srsran/f1ap/du/f1ap_du.h"
#include <memory>

namespace srsran {

class timer_manager;

namespace srs_du {

class du_high_ue_executor_mapper;
class f1c_connection_client;

/// Creates an instance of an F1AP DU interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_du> create_f1ap(f1c_connection_client&      f1c_client_handler,
                                     f1ap_du_configurator&       du_mng,
                                     task_executor&              ctrl_exec,
                                     du_high_ue_executor_mapper& ue_exec_mapper,
                                     f1ap_du_paging_notifier&    paging_notifier,
                                     srsran::timer_manager&      timers);

} // namespace srs_du
} // namespace srsran
