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

#include "../adapters/ngap_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../cell_meas_manager/measurement_context.h"
#include "cu_cp_ue_impl.h"
#include "ue_metrics_handler.h"
#include "ue_task_scheduler_impl.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/security_manager_config.h"
#include "srsran/cu_cp/ue_configuration.h"
#include "srsran/ran/plmn_identity.h"
#include <optional>
#include <set>
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

class ue_manager : public ue_metrics_handler
{
public:
  explicit ue_manager(const cu_cp_configuration& cu_cp_cfg);

  /// Stop UE activity.
  void stop();

  /// \brief Remove the UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  void remove_ue(ue_index_t ue_index);

  bool set_plmn(ue_index_t ue_index, const plmn_identity& plmn);

  /// \brief Add PLMNs to block and reject new UE connections for these.
  /// \param[in] plmns PLMN identities of UEs to be rejected.
  void add_blocked_plmns(const std::vector<plmn_identity>& plmns);

  /// \brief Remove blocked PLMNs and re-allow new UE connections for these.
  /// \param[in] plmns PLMN identities of the UEs to be allowed again.
  void remove_blocked_plmns(const std::vector<plmn_identity>& plmns);

  /// \brief Find the UEs with the given PLMN identity.
  /// \param[in] plmn PLMN identity of the UEs to be found.
  /// \return Vector of pointers to the found UEs.
  std::vector<cu_cp_ue*> find_ues(plmn_identity plmn);

  /// \brief Get the UE index of the UE.
  /// \param[in] pci The PCI of the cell the UE is/was connected to.
  /// \param[in] c_rnti The RNTI of the UE.
  ue_index_t get_ue_index(pci_t pci, rnti_t c_rnti);

  /// \brief Get the CU-CP UE configuration stored in the UE manager.
  /// \return The CU-CP UE configuration.
  const ue_configuration& get_ue_config() const { return ue_config; }

  /// \brief Get the number of UEs.
  /// \return Number of UEs.
  size_t get_nof_ues() const { return ues.size(); }

  // common

  /// \brief Find the UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the UE if found, nullptr otherwise.
  cu_cp_ue* find_ue(ue_index_t ue_index);

  /// \brief Get the UE task scheduler of the specified UE, even if the DU UE context is not created.
  /// \param[in] ue_index Index of the UE.
  /// \return Pointer to the UE task scheduler if found, nullptr otherwise.
  ue_task_scheduler* find_ue_task_scheduler(ue_index_t ue_index);

  // du processor

  /// \brief Allocate resources for the UE in the CU-CP.
  /// \param[in] du_index Index of the DU the UE is connected to.
  /// \param[in] du_id The gNB-DU ID of the DU the UE is connected to.
  /// \param[in] pci The PCI of the cell the UE is connected to.
  /// \param[in] rnti The RNTI of the UE.
  /// \param[in] pcell_index The index of the PCell the UE is connected to.
  /// \return ue_index of the created UE or ue_index_t::invalid in case of failure.
  ue_index_t add_ue(du_index_t                     du_index,
                    std::optional<gnb_du_id_t>     du_id       = std::nullopt,
                    std::optional<pci_t>           pci         = std::nullopt,
                    std::optional<rnti_t>          rnti        = std::nullopt,
                    std::optional<du_cell_index_t> pcell_index = std::nullopt);

  /// \brief Set the DU context of the UE.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] du_id The gNB-DU ID of the DU the UE is connected to.
  /// \param[in] pci The PCI of the cell the UE is connected to.
  /// \param[in] rnti The RNTI of the UE.
  /// \param[in] pcell_index The index of the PCell the UE is connected to.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  cu_cp_ue*
  set_ue_du_context(ue_index_t ue_index, gnb_du_id_t du_id, pci_t pci, rnti_t rnti, du_cell_index_t pcell_index);

  /// \brief Find the UE with the given UE index, thats DU context is set up.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  cu_cp_ue* find_du_ue(ue_index_t ue_index);

  /// \brief Get the number of UEs connected to a specific DU.
  /// \return Number of UEs.
  size_t get_nof_du_ues(du_index_t du_index);

  // ngap

  // cu-cp ue manager
  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_ngap_rrc_ue_adapter();
  }

  rrc_ue_ngap_adapter& get_rrc_ue_ngap_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_rrc_ue_ngap_adapter();
  }

  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_rrc_ue_cu_cp_adapter();
  }

  std::vector<cu_cp_metrics_report::ue_info> handle_ue_metrics_report_request() const override;

  ue_task_scheduler_manager& get_task_sched() { return ue_task_scheds; }

  cell_meas_manager_ue_context& get_measurement_context(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_meas_context();
  }

protected:
  ue_index_t next_ue_index = ue_index_t::min;

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t allocate_ue_index();

  inline void increase_next_ue_index()
  {
    if (next_ue_index == ue_index_t::max) {
      // reset cu ue f1ap id counter
      next_ue_index = ue_index_t::min;
    } else {
      // increase cu ue f1ap id counter
      next_ue_index = uint_to_ue_index(ue_index_to_uint(next_ue_index) + 1);
    }
  }

  srslog::basic_logger&         logger = srslog::fetch_basic_logger("CU-UEMNG");
  const cu_cp_configuration     cu_cp_config;
  const ue_configuration        ue_config;
  const up_resource_manager_cfg up_config;
  const security_manager_config sec_config;
  const unsigned                max_nof_ues;

  // Manager of UE task schedulers.
  ue_task_scheduler_manager ue_task_scheds;

  // Container of UE contexts handled by the CU-CP.
  std::unordered_map<ue_index_t, cu_cp_ue> ues;

  // ue index lookups
  std::map<std::tuple<pci_t, rnti_t>, ue_index_t> pci_rnti_to_ue_index; // ue_indexes indexed by pci and rnti

  std::set<plmn_identity> blocked_plmns;
};

} // namespace srs_cu_cp
} // namespace srsran
