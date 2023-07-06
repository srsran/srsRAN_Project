/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mobility_manager_config.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/pci.h"

namespace srsran {
namespace srs_cu_cp {

/// Methods used by cell measurement manager to signal measurement events to the mobility manager.
class cell_meas_mobility_manager_notifier
{
public:
  virtual ~cell_meas_mobility_manager_notifier() = default;

  /// \brief Notifies that a neighbor cell became stronger than the current serving cell.
  virtual void on_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

/// Object to manage mobility. An instance of this class resides in the CU-CP and handles all kinds of events that might
/// trigger the change of the serving cell of a user. It consumes (measurement) events from local cells as well as from
/// cells not managed by the CU-CP itself. As such it checks the requests and dispatches them to perform:
/// * Intra DU handover (delegate to DU processor)
/// * Inter DU handover (delegate to CU-CP)
/// * Inter CU handover over N2 (delegate to CU-CP/NGAP)
class mobility_manager
{
public:
  virtual ~mobility_manager() = default;

  /// \brief Handle event where neighbor became better than serving cell.
  virtual void handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

/// Creates an instance of an cell measurement manager.
std::unique_ptr<mobility_manager> create_mobility_manager(const mobility_manager_cfg& cfg);

} // namespace srs_cu_cp
} // namespace srsran
