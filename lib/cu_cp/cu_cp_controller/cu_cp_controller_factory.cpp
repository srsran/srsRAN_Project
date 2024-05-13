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

#include "cu_cp_controller_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<cu_cp_controller> srsran::srs_cu_cp::create_cu_cp_controller(cu_cp_routine_manager&    routine_mng,
                                                                             const ngap_configuration& ngap_cfg,
                                                                             ngap_connection_manager&  ngap_conn_mng,
                                                                             const cu_up_processor_repository& cu_ups)
{
  auto cu_cp_ctrl = std::make_unique<cu_cp_controller>(routine_mng, ngap_cfg, ngap_conn_mng, cu_ups);
  return cu_cp_ctrl;
}
