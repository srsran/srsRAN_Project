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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_du {

class f1ap_du_configurator;
class f1ap_du_ue;

/// Initiate F1AP Positioning Information Exchange Procedure as per TS 38.473, Section 8.13.9.
async_task<void> start_positioning_exchange_procedure(const asn1::f1ap::positioning_info_request_s& msg,
                                                      f1ap_du_configurator&                         du_mng,
                                                      f1ap_du_ue&                                   ue);

} // namespace srsran::srs_du
