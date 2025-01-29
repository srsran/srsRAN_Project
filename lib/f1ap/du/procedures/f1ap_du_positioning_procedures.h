/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class f1ap_message_notifier;

namespace srs_du {

class f1ap_du_configurator;
class f1ap_du_ue;

/// Initiate F1AP Positioning Information Exchange Procedure as per TS 38.473, Section 8.13.3.
async_task<void> start_positioning_measurement_procedure(const asn1::f1ap::positioning_meas_request_s& msg,
                                                         f1ap_du_configurator&                         du_mng,
                                                         f1ap_message_notifier&                        cu_notifier);

/// Initiate F1AP TRP Information Exchange Procedure as per TS 38.473, Section 8.13.8.
async_task<void> start_trp_information_exchange_procedure(const asn1::f1ap::trp_info_request_s& msg,
                                                          f1ap_du_configurator&                 du_mng,
                                                          f1ap_message_notifier&                cu_notifier);

/// Initiate F1AP Positioning Information Exchange Procedure as per TS 38.473, Section 8.13.9.
async_task<void> start_positioning_exchange_procedure(const asn1::f1ap::positioning_info_request_s& msg,
                                                      f1ap_du_configurator&                         du_mng,
                                                      f1ap_du_ue&                                   ue);
} // namespace srs_du
} // namespace srsran
