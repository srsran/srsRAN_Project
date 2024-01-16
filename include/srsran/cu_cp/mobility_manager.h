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

#include "du_repository.h"
#include "mobility_manager_config.h"
#include "ue_manager.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/pci.h"

namespace srsran {
namespace srs_cu_cp {

/// Handler for measurement related events.
class mobility_manager_measurement_handler
{
public:
  virtual ~mobility_manager_measurement_handler() = default;

  /// \brief Handle event where neighbor became better than serving cell.
  virtual void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

/// Object to manage mobility. An instance of this class resides in the CU-CP and handles all kinds of events that might
/// trigger the change of the serving cell of a user. It consumes (measurement) events from local cells as well as from
/// cells not managed by the CU-CP itself. As such it checks the requests and dispatches them to perform:
/// * Intra DU handover (delegate to DU processor)
/// * Inter DU handover (delegate to CU-CP)
/// * Inter CU handover over N2 (delegate to CU-CP/NGAP)
class mobility_manager : public mobility_manager_measurement_handler
{
public:
  virtual ~mobility_manager() = default;
};

/// Creates an instance of an cell measurement manager.
std::unique_ptr<mobility_manager>
create_mobility_manager(const mobility_manager_cfg& cfg_, du_repository& du_db_, du_processor_ue_manager& ue_mng_);

} // namespace srs_cu_cp
} // namespace srsran
