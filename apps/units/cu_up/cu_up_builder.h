/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_up/cu_up.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {

namespace srs_cu_up {
class e1_connection_client;
}

struct cu_up_unit_config;
class dlt_pcap;
class f1u_cu_up_gateway;
class io_broker;
struct worker_manager;

/// Builds the CU UP using the given arguments.
std::unique_ptr<srs_cu_up::cu_up_interface> build_cu_up(const cu_up_unit_config&         unit_cfg,
                                                        worker_manager&                  workers,
                                                        srs_cu_up::e1_connection_client& e1ap_conn_client,
                                                        f1u_cu_up_gateway&               f1u_gateway,
                                                        dlt_pcap&                        gtpu_pcap,
                                                        timer_manager&                   timers,
                                                        io_broker&                       io_brk);

} // namespace srsran
