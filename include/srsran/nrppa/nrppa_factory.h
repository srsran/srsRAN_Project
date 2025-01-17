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

#include "srsran/cu_cp/common_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/nrppa/nrppa.h"

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an NRPPA interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<nrppa_interface> create_nrppa(const cu_cp_configuration& cfg,
                                              nrppa_cu_cp_notifier&      cu_cp_notifier,
                                              common_task_scheduler&     common_task_sched);

} // namespace srs_cu_cp
} // namespace srsran
