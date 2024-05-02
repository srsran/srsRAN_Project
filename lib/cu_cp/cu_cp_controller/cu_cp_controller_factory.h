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

#include "cu_cp_controller.h"

namespace srsran {

namespace srs_cu_cp {

/// Creates an instance of a CU-CP controller.
std::unique_ptr<cu_cp_controller> create_cu_cp_controller(cu_cp_routine_manager&            routine_mng,
                                                          const ngap_configuration&         ngap_cfg,
                                                          ngap_connection_manager&          ngap_conn_mng,
                                                          const cu_up_processor_repository& cu_ups);

} // namespace srs_cu_cp
} // namespace srsran
