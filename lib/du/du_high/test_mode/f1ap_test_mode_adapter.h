/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// Creates an F1AP handler for the DU-high.
std::unique_ptr<f1ap_du> create_du_high_f1ap(f1c_connection_client&      f1c_client_handler,
                                             f1ap_du_configurator&       du_mng,
                                             task_executor&              ctrl_exec,
                                             du_high_ue_executor_mapper& ue_exec_mapper,
                                             f1ap_du_paging_notifier&    paging_notifier,
                                             timer_manager&              timers,
                                             const du_test_mode_config&  test_cfg);

} // namespace srs_du
} // namespace srsran
