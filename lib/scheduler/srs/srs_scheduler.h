/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

struct cell_resource_allocator;
class ue_cell_configuration;

/// SRS scheduling interface, which handles the scheduling of SRS opportunities.
class srs_scheduler
{
public:
  virtual ~srs_scheduler() = default;

  /// Schedules the SRS occasions.
  /// \param[out,in] res_alloc struct with scheduling results.
  virtual void run_slot(cell_resource_allocator& res_alloc) = 0;

  /// Adds a UE to the internal list of UEs to be scheduled.
  /// \param[in] ue_cfg dedicated configuration of the UE to be added.
  virtual void add_ue(const ue_cell_configuration& ue_cfg) = 0;

  /// Removes the UE from the internal list of UEs to be scheduled.
  /// \param[in] ue_cfg UE dedicated configuration of the UE to be removed.
  virtual void rem_ue(const ue_cell_configuration& ue_cfg) = 0;

  /// Updates the SRS configuration of this UE, if there are any changes w.r.t. the previous configuration.
  /// \param[in] ue_cfg New UE dedicated configuration of the UE to be reconfigured.
  /// \param[in] ue_cfg Old UE dedicated configuration of the UE to be reconfigured.
  virtual void reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration& old_ue_cfg) = 0;
};

} // namespace srsran
