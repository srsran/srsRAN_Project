/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/cause/common.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/rrc/rrc_cell_context.h"
#include "srsran/rrc/rrc_metrics.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// RRC DU cell information extracted from the SIB1 message.
struct rrc_cell_info {
  nr_band                      band;
  pci_t                        nr_pci;
  std::vector<rrc_meas_timing> meas_timings;
  rrc_timers_t                 timers;
  // > PLMN identities broadcasted in SIB1. Max 12 as per TS 38.331 section 6.4.
  std::vector<plmn_identity> plmn_identity_list;
};

class rrc_du_cell_manager
{
public:
  rrc_du_cell_manager()          = default;
  virtual ~rrc_du_cell_manager() = default;

  /// \brief Get information about the cells served by this DU.
  /// \param[in] served_cell_list The list of served cells received from the DU in the F1 Setup Request.
  /// \return A map of cell id to cell information for all served cells or an empty map on error.
  virtual std::map<nr_cell_global_id_t, rrc_cell_info>
  get_cell_info(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) const = 0;

  virtual void store_cell_info_db(const std::map<nr_cell_global_id_t, rrc_cell_info>& cell_infos) = 0;
};

struct rrc_ue_creation_message {
  ue_index_t                             ue_index;
  rnti_t                                 c_rnti;
  rrc_cell_context                       cell;
  rrc_pdu_f1ap_notifier*                 f1ap_pdu_notifier;
  rrc_ue_ngap_notifier*                  ngap_notifier;
  rrc_ue_context_update_notifier*        rrc_ue_cu_cp_notifier;
  rrc_ue_measurement_notifier*           measurement_notifier;
  rrc_ue_cu_cp_ue_notifier*              cu_cp_ue_notifier;
  byte_buffer                            du_to_cu_container;
  std::optional<rrc_ue_transfer_context> rrc_context;
};

/// \brief Interface class to the main RRC DU object to manage RRC UEs.
/// This interface provides functions to add, remove and release UEs.
class rrc_du_ue_repository
{
public:
  rrc_du_ue_repository()          = default;
  virtual ~rrc_du_ue_repository() = default;

  /// \brief Get the RRC Reject message to send to the UE.
  virtual byte_buffer get_rrc_reject() = 0;

  /// Creates a new RRC UE object and returns a handle to it.
  virtual rrc_ue_interface* add_ue(const rrc_ue_creation_message& msg) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void release_ues() = 0;

  /// \brief Get the number of UEs registered at the RRC DU.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Handle RRC UE removal
class rrc_ue_handler
{
public:
  virtual ~rrc_ue_handler() = default;

  virtual rrc_ue_interface* find_ue(ue_index_t ue_index) = 0;

  /// Remove a RRC UE object.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void remove_ue(ue_index_t ue_index) = 0;
};

class rrc_du_connection_event_handler
{
public:
  virtual ~rrc_du_connection_event_handler() = default;

  /// \brief Add the successful RRC setup to the metrics.
  /// \param[in] cause The establishment cause of the RRC connection. If this is given the connection establishment
  /// metrics are increased. Otherwise the connection metrics are increased.
  virtual void handle_successful_rrc_setup(std::optional<establishment_cause_t> cause = std::nullopt) = 0;

  /// \brief Add the successful RRC release to the metrics.
  virtual void handle_successful_rrc_release() = 0;

  /// \brief Add the attempted RRC connection establishment to the metrics.
  /// \param[in] cause The establishment cause of the RRC connection.
  virtual void handle_attempted_rrc_setup(establishment_cause_t cause) = 0;

  /// \brief Add the attempted RRC connection re-establishment to the metrics.
  virtual void handle_attempted_rrc_reestablishment() = 0;

  /// \brief Add the successful RRC connection re-establishment to the metrics.
  virtual void handle_successful_rrc_reestablishment() = 0;

  /// \brief Add the successful RRC connection re-establishment fallback to the metrics.
  virtual void handle_successful_rrc_reestablishment_fallback() = 0;
};

class rrc_du_metrics_collector
{
public:
  virtual ~rrc_du_metrics_collector() = default;

  /// \brief Collect the metrics of this RRC DU.
  /// \param[out] metrics The metrics to collect.
  virtual void collect_metrics(rrc_du_metrics& metrics) = 0;
};

/// Combined entry point for the RRC DU handling.
class rrc_du : public rrc_du_cell_manager,
               public rrc_du_ue_repository,
               public rrc_ue_handler,
               public rrc_du_connection_event_handler
{
public:
  virtual ~rrc_du() = default;

  virtual rrc_du_cell_manager&             get_rrc_du_cell_manager()             = 0;
  virtual rrc_du_ue_repository&            get_rrc_du_ue_repository()            = 0;
  virtual rrc_ue_handler&                  get_rrc_ue_handler()                  = 0;
  virtual rrc_du_connection_event_handler& get_rrc_du_connection_event_handler() = 0;
  virtual rrc_du_metrics_collector&        get_rrc_du_metrics_collector()        = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
