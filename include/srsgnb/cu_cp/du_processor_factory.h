/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_processor.h"
#include "du_processor_config.h"
#include "ue_manager.h"
#include "srsgnb/f1ap/common/f1c_common.h"
#include "srsgnb/rrc/rrc_ue.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

/// Creates an instance of an DU processor interface
std::unique_ptr<du_processor_interface> create_du_processor(const du_processor_config_t     du_processor_config_,
                                                            du_processor_cu_cp_notifier&    cu_cp_notifier_,
                                                            f1c_du_management_notifier&     f1c_du_mgmt_notifier_,
                                                            f1c_message_notifier&           f1c_notifier_,
                                                            rrc_ue_e1_control_notifier&     rrc_ue_e1_ctrl_notifier_,
                                                            rrc_ue_nas_notifier&            rrc_ue_nas_pdu_notifier_,
                                                            rrc_ue_control_notifier&        rrc_ue_ngc_ctrl_notifier_,
                                                            du_processor_ue_task_scheduler& task_sched_,
                                                            du_processor_ue_manager&        ue_manager_);

} // namespace srs_cu_cp
} // namespace srsgnb
