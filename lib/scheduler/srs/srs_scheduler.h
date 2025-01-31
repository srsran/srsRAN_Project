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
struct positioning_measurement_request;

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

  /// \brief Handles a new request to measure the position of a UE.
  ///
  /// The UE whose position is being measured may either be connected the current cell or to a neighbor cell.
  /// \param[in] req Request for positioning measurement.
  virtual void handle_positioning_measurement_request(const positioning_measurement_request& req) = 0;

  /// Handles a stop of a positioning measurement that is on-going.
  /// \param[in] Cell where the positioning measurement takes place.
  /// \param[in] Identifier of the measurement procedure that is on-going.
  virtual void handle_positioning_measurement_stop(du_cell_index_t cell_index, rnti_t pos_rnti) = 0;
};

} // namespace srsran
