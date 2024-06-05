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

#include "srsran/cu_up/cu_up.h"
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
