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

#include "adapters/ngap_adapters.h"
#include "task_schedulers/ngap_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/gateways/n2_connection_client.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ran/plmn_identity.h"

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_configuration;

struct ngap_repository_config {
  const cu_cp_configuration& cu_cp;
  cu_cp_ngap_handler&        cu_cp_notifier;
  paging_message_handler&    paging_handler;
  srslog::basic_logger&      logger;
};

class ngap_repository
{
public:
  explicit ngap_repository(ngap_repository_config cfg_);

  /// \brief Adds a NGAP object to the CU-CP.
  /// \return A pointer to the interface of the added NGAP object if it was successfully created, a nullptr otherwise.
  ngap_interface* add_ngap(amf_index_t amf_index, const cu_cp_configuration::ngap_config& config);

  /// \brief Updates the PLMN lookup table with the PLMNs supported by the connected NGAP.
  /// \param[in] amf_index The AMF index to identify the NGAP.
  void update_plmn_lookup(amf_index_t amf_index);

  /// \brief Checks whether a AMF with the specified PLMN is served by any of the connected NGAPs.
  /// \param[in] plmn The PLMN to identify the NGAP.
  /// \return The interface of the NGAP for the given PLMN if it is found, nullptr if no NGAP for the PLMN is found.
  ngap_interface* find_ngap(const plmn_identity& plmn);

  /// \brief Checks whether a AMF with the specified AMF index is in the connected NGAPs.
  /// \param[in] amf_index The AMF index to identify the NGAP.
  /// \return The interface of the NGAP for the given AMF index if it is found, nullptr not.
  ngap_interface* find_ngap(const amf_index_t& amf_index);

  /// \brief Get the all NGAP interfaces.
  std::map<amf_index_t, ngap_interface*> get_ngaps();

  ngap_task_scheduler& get_ngap_task_scheduler() { return amf_task_sched; }

  /// Number of NGAPs managed by the CU-CP.
  size_t get_nof_ngaps() const { return ngap_db.size(); }

  /// Number of UEs managed by the CU-CP.
  size_t get_nof_ngap_ues();

private:
  struct ngap_context {
    // CU-CP handler of NGAP events.
    ngap_cu_cp_adapter ngap_to_cu_cp_notifier;

    std::unique_ptr<ngap_interface> ngap;

    /// Notifier used by the CU-CP to push NGAP Tx messages to the respective AMF.
    std::unique_ptr<ngap_message_notifier> ngap_tx_pdu_notifier;
  };

  ngap_repository_config cfg;
  srslog::basic_logger&  logger;

  ngap_task_scheduler amf_task_sched;

  std::unordered_map<plmn_identity, amf_index_t> plmn_to_amf_index;
  std::map<amf_index_t, ngap_context>            ngap_db;
};

} // namespace srs_cu_cp
} // namespace srsran
