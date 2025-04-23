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

#include "du_ue_resource_config.h"
#include "ue_capability_summary.h"
#include "srsran/f1ap/du/f1ap_du_ue_context_update.h"

namespace srsran {
namespace srs_du {

/// \brief Outcome report of an DU UE Resource allocation request.
struct du_ue_resource_update_response {
  /// \brief Defines whether the UE resource allocation failed during the config update procedure.
  /// If \c procedure_error doesn't contain any error string, then the UE resource update was successful.
  error_type<std::string>        procedure_error = {};
  std::vector<drb_id_t>          failed_drbs;
  std::vector<serv_cell_index_t> failed_scells;

  bool failed() const { return not procedure_error.has_value(); }
};

/// \brief This class manages the PHY (e.g. RB and symbols used for PUCCH), MAC (e.g. LCIDs) and RLC resources used
/// by an UE. It provides an API to update the UE resources on arrival of new UE Context Update Requests, and
/// returns resources back to the DU RAN Resource Manager when the UE is destroyed.
class ue_ran_resource_configurator
{
public:
  /// \brief Interface used to update the UE Resources on Reconfiguration and return the resources back to the pool,
  /// on UE deletion.
  struct resource_updater {
    virtual ~resource_updater() = default;

    /// Called when a new UE configuration is requested.
    virtual du_ue_resource_update_response update(du_cell_index_t                       pcell_index,
                                                  const f1ap_ue_context_update_request& upd_req,
                                                  const du_ue_resource_config*          reestablished_context,
                                                  const ue_capability_summary*          reestablished_ue_caps) = 0;

    /// Called when the UE confirms the configuration is applied.
    virtual void config_applied() = 0;

    /// Called to fetch the UE resources.
    virtual const du_ue_resource_config& get() = 0;

    /// Called to fetch the UE capabilities.
    virtual const std::optional<ue_capability_summary>& ue_capabilities() const = 0;
  };

  explicit ue_ran_resource_configurator(std::unique_ptr<resource_updater> ue_res_, std::string error = {}) :
    ue_res_impl(std::move(ue_res_)),
    cached_res(&ue_res_impl->get()),
    cached_ue_caps(&ue_res_impl->ue_capabilities()),
    configurator_error(std::move(error))
  {
  }

  /// \brief Initiates the update of the resources (PCell, SCells, Bearers) used by the UE.
  ///
  /// \param pcell_index DU Cell Index of the UE's PCell.
  /// \param upd_req UE Context Update Request for a given UE.
  /// \param reestablished_context Optional parameter to provide the previous context of the UE, in case of an
  /// RRC Reestablishment.
  /// \param reestablished_ue_caps Optional parameter to provide the previous capabilities of the UE, in case of an
  /// RRC Reestablishment.
  /// \return Outcome of the configuration.
  du_ue_resource_update_response update(du_cell_index_t                       pcell_index,
                                        const f1ap_ue_context_update_request& upd_req,
                                        const du_ue_resource_config*          reestablished_context = nullptr,
                                        const ue_capability_summary*          reestablished_ue_caps = nullptr)
  {
    return ue_res_impl->update(pcell_index, upd_req, reestablished_context, reestablished_ue_caps);
  }

  /// \brief Checks whether the allocation of resources to the UE failed, due to lack of resources.
  bool resource_alloc_failed() const { return not configurator_error.empty(); }

  /// \brief Handle the confirmation from that UE that the last RAN resource configuration is complete.
  void handle_ue_config_applied() { ue_res_impl->config_applied(); }

  /// \brief Returns the configurator error, which non-empty string only if the procedure failed.
  std::string get_error() const { return configurator_error; }

  const du_ue_resource_config& value() const { return *cached_res; }
  const du_ue_resource_config& operator*() const { return *cached_res; }
  const du_ue_resource_config* operator->() const { return cached_res; }

  /// Retrieve UE capabilities.
  const std::optional<ue_capability_summary>& get_ue_capabilities() const { return *cached_ue_caps; }

private:
  std::unique_ptr<resource_updater>           ue_res_impl;
  const du_ue_resource_config*                cached_res;
  const std::optional<ue_capability_summary>* cached_ue_caps;
  std::string                                 configurator_error;
};

/// \brief This class creates new UE resource configs (PHY, MAC and RLC), using a specific pool of DU resources.
class du_ran_resource_manager
{
public:
  virtual ~du_ran_resource_manager() = default;

  /// \brief Create a new UE resource allocation config object.
  /// \return UE Resource configuration if correctly created. Unexpected if no space in the manager was found.
  virtual expected<ue_ran_resource_configurator, std::string>
  create_ue_resource_configurator(du_ue_index_t ue_index, du_cell_index_t pcell_index, bool has_tc_rnti) = 0;
};

} // namespace srs_du
} // namespace srsran
