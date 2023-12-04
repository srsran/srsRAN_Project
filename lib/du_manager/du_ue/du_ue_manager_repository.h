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

#include "du_ue.h"
#include "srsran/gtpu/gtpu_teid_pool.h"

namespace srsran {
namespace srs_du {

/// Allows accessing and interacting with the state of a DU UE.
class du_ue_controller
{
public:
  virtual ~du_ue_controller() = default;

  /// \brief Disconnect the UE inter-layer notifiers.
  ///
  /// This method should be called as a first step in the deletion of a UE, to ensure traffic is not flowing through
  /// its bearers during the layer by layer UE context removal.
  virtual async_task<void> disconnect_notifiers() = 0;

  /// \brief Schedule task for a given UE.
  virtual void schedule_async_task(async_task<void> task) = 0;

  /// \brief Handle detection of RLF in MAC or RLC.
  virtual void handle_rlf_detection(rlf_cause cause) = 0;

  /// \brief Handle the detection of a C-RNTI MAC CE for this UE.
  virtual void handle_crnti_ce_detection() = 0;

  /// \brief Handle command to activate/deactivate scheduling for this UE.
  virtual void handle_scheduling_activation(bool active) = 0;

  /// \brief Access to the MAC RLF notifier for this UE.
  virtual mac_ue_radio_link_notifier& get_mac_rlf_notifier() = 0;

  /// \brief Access to the RLC RLF notifier for this UE.
  virtual rlc_tx_upper_layer_control_notifier& get_rlc_rlf_notifier() = 0;
};

/// \brief Interface used by DU Manager procedures to add/find/remove UEs from the DU UE Manager.
class du_ue_manager_repository
{
public:
  virtual ~du_ue_manager_repository() = default;

  /// \brief Adds a new UE context in the DU UE manager repository.
  virtual du_ue* add_ue(std::unique_ptr<du_ue> u) = 0;

  /// \brief removes an existing ue context from the du ue manager repository.
  virtual void remove_ue(du_ue_index_t ue_index) = 0;

  /// \brief Update the CRNTI of an existing UE context.
  virtual void update_crnti(du_ue_index_t ue_index, rnti_t crnti) = 0;

  /// \brief Find UE context based on DU UE index.
  virtual du_ue*       find_ue(du_ue_index_t ue_index)       = 0;
  virtual const du_ue* find_ue(du_ue_index_t ue_index) const = 0;

  /// \brief Find UE context based on UE RNTI.
  virtual du_ue* find_rnti(rnti_t rnti) = 0;

  /// \brief Find UE context based on GNB-DU-UE-F1AP-ID.
  virtual du_ue* find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id) = 0;

  /// \brief Handle UE release due to detected Radio Link Failures.
  virtual void handle_rlf_ue_release(du_ue_index_t ue_index, rlf_cause cause) = 0;

  /// \brief Access to the TEID pool that can be used to allocate/deallocate unique TEIDs for F1-U bearers.
  virtual gtpu_teid_pool& get_f1u_teid_pool() = 0;

  /// \brief Schedule task for a given UE.
  virtual void schedule_async_task(du_ue_index_t ue_index, async_task<void> task) = 0;

  /// \brief Access to the DU UE controller based on DU UE index.
  virtual du_ue_controller& get_ue_controller(du_ue_index_t ue_index) = 0;
};

} // namespace srs_du
} // namespace srsran
