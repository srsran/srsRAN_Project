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

#include "rrc_cell_context.h"
#include "rrc_ue.h"
#include "srsran/ran/band_helper.h"

namespace srsran {

namespace srs_cu_cp {

struct rrc_cell_info {
  nr_band                      band;
  std::vector<rrc_meas_timing> meas_timings;
};

class rrc_du_cell_manager
{
public:
  rrc_du_cell_manager()          = default;
  virtual ~rrc_du_cell_manager() = default;

  virtual bool handle_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) = 0;
};

struct rrc_ue_creation_message {
  ue_index_t             ue_index;
  rnti_t                 c_rnti;
  rrc_cell_context       cell;
  rrc_pdu_f1ap_notifier* f1ap_pdu_notifier;
  byte_buffer            du_to_cu_container;
  rrc_ue_task_scheduler* ue_task_sched;
  bool                   is_inter_cu_handover = false;
};

/// \brief Interface class for the main RRC DU object used by the RRC UE objects.
/// This interface provides the RRC connection permission.
class rrc_du_ue_manager
{
public:
  rrc_du_ue_manager()          = default;
  virtual ~rrc_du_ue_manager() = default;

  /// Check if the parent allows RRC connections.
  virtual bool is_rrc_connect_allowed() = 0;
};

/// \brief Interface class to the main RRC DU object to manage RRC UEs.
/// This interface provides functions to add, remove and release UEs.
class rrc_du_ue_repository : public rrc_amf_connection_handler
{
public:
  rrc_du_ue_repository()          = default;
  virtual ~rrc_du_ue_repository() = default;

  /// Creates a new RRC UE object and returns a handle to it.
  virtual rrc_ue_interface* add_ue(up_resource_manager& resource_mng, const rrc_ue_creation_message msg) = 0;

  /// Get a RRC UE object.
  virtual rrc_ue_interface* find_ue(ue_index_t ue_index) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void release_ues() = 0;
};

/// Handle RRC UE removal
class rrc_ue_removal_handler
{
public:
  virtual ~rrc_ue_removal_handler() = default;

  /// Remove a RRC UE object.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

/// \brief Interface to query statistics from the RRC DU interface.
class rrc_du_statistics_handler
{
public:
  virtual ~rrc_du_statistics_handler() = default;

  /// \brief Get the number of UEs registered at the RRC DU.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Combined entry point for the RRC DU handling.
class rrc_du_interface : public rrc_du_cell_manager,
                         public rrc_du_ue_manager,
                         public rrc_du_ue_repository,
                         public rrc_ue_removal_handler,
                         public rrc_du_statistics_handler
{
public:
  virtual ~rrc_du_interface() = default;

  virtual rrc_du_cell_manager&       get_rrc_du_cell_manager()       = 0;
  virtual rrc_du_ue_manager&         get_rrc_du_ue_manager()         = 0;
  virtual rrc_du_ue_repository&      get_rrc_du_ue_repository()      = 0;
  virtual rrc_ue_removal_handler&    get_rrc_ue_removal_handler()    = 0;
  virtual rrc_du_statistics_handler& get_rrc_du_statistics_handler() = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
