/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ngap/ngap_nrppa.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<void> start_ngap_dl_non_ue_associated_nrppa_transport(const ngap_non_ue_associated_nrppa_transport& msg,
                                                                 srslog::basic_logger&                         logger);

} // namespace srs_cu_cp
} // namespace srsran
