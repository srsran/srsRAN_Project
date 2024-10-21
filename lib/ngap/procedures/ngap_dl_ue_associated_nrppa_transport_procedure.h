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

#include "ue_context/ngap_ue_logger.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_nrppa.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<void> start_ngap_dl_ue_associated_nrppa_transport(const ngap_ue_associated_nrppa_transport& msg,
                                                             ngap_cu_cp_notifier&                      cu_cp_notifier,
                                                             ngap_message_notifier&                    amf_notifier,
                                                             ngap_ue_logger&                           logger);

} // namespace srs_cu_cp
} // namespace srsran
