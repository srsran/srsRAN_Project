/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"

namespace srsran {
namespace ofh {

/// OTA symbol receive handler.
class ota_rx_symbol_handler : public ota_symbol_boundary_notifier
{
public:
  ota_rx_symbol_handler(uplink_cplane_context_repository&            cp_context_repo_,
                        uplink_context_repository<ul_prach_context>& prach_repo_,
                        uplink_context_repository<ul_slot_context>&  ul_slot_repo_) :
    cp_context_repo(cp_context_repo_), prach_repo(prach_repo_), ul_slot_repo(ul_slot_repo_)
  {
  }

  // See interface for documentation.
  void on_new_symbol(slot_point slot, unsigned symbol_index) override;

private:
  /// Clears the given slot in the registered repositories.
  void clear_slot_in_repositories(slot_point slot);

private:
  uplink_cplane_context_repository&            cp_context_repo;
  uplink_context_repository<ul_prach_context>& prach_repo;
  uplink_context_repository<ul_slot_context>&  ul_slot_repo;
  slot_point                                   current_slot;
};

} // namespace ofh
} // namespace srsran
