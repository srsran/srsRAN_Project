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

#include "mobility_manager_impl.h"

namespace srsran {

namespace srs_cu_cp {

/// Creates an instance of a mobility manager.
std::unique_ptr<mobility_manager> create_mobility_manager(const mobility_manager_cfg&      cfg,
                                                          mobility_manager_cu_cp_notifier& cu_cp_notifier,
                                                          ngap_repository&                 ngap_db,
                                                          du_processor_repository&         du_db,
                                                          ue_manager&                      ue_mng);

} // namespace srs_cu_cp
} // namespace srsran
