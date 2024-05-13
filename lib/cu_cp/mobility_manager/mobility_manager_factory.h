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

#include "mobility_manager_impl.h"

namespace srsran {

namespace srs_cu_cp {

/// Creates an instance of a mobility manager.
std::unique_ptr<mobility_manager> create_mobility_manager(const mobility_manager_cfg&      cfg,
                                                          mobility_manager_cu_cp_notifier& cu_cp_notifier,
                                                          ngap_control_message_handler&    ngap_handler,
                                                          du_processor_repository&         du_db,
                                                          ue_manager&                      ue_mng);

} // namespace srs_cu_cp
} // namespace srsran
