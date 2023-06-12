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
  void on_new_symbol(slot_symbol_point symbol_point) override;

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
