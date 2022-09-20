/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/f1ap_adapters.h"
#include "adapters/ngap_adapters.h"
#include "du_processor_impl.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

class cu_cp final : public cu_cp_interface
{
public:
  explicit cu_cp(const cu_cp_configuration& cfg_);
  ~cu_cp();

  void start();
  void stop();

  f1c_message_handler& get_f1c_message_handler(du_index_t du_index) override;

  f1c_statistics_handler& get_f1c_statistics_handler(du_index_t du_index) override;

  ngc_message_handler& get_ngc_message_handler() override;

  bool amf_is_connected() override { return amf_connected; };

  void on_new_connection() override;
  void handle_du_remove_request(const du_index_t du_index) override;

  // ngc_connection_notifier
  void on_amf_connection() override;
  void on_amf_connection_drop() override;

  // CU-CP statistics
  size_t get_nof_dus() const override;
  size_t get_nof_ues() const override;

private:
  /// \brief Adds a DU processor object to the CU-CP.
  /// \return The DU index of the added DU processor object.
  du_index_t add_du();

  /// \brief Removes the specified DU processor object from the CU-CP.
  /// \param[in] du_index The index of the DU processor to delete.
  void remove_du(du_index_t du_index);

  /// \brief Find a DU object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The DU processor object.
  du_processor_interface& find_du(du_index_t du_index);

  /// \brief Get the next available index from the DU processor database.
  /// \return The DU index.
  du_index_t get_next_du_index();

  cu_cp_configuration cfg;
  timer_manager       timers;

  // Handler for DU tasks.
  async_task_sequencer main_ctrl_loop;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");

  // Components
  std::unique_ptr<ngc_interface> ngc_entity;

  slot_array<std::unique_ptr<du_processor_interface>, MAX_NOF_DUS> du_db;

  // task event loops indexed by du_index
  slot_array<async_task_sequencer, MAX_NOF_DUS> du_ctrl_loop;

  // F1AP to CU-CP adapter
  f1ap_cu_cp_adapter f1ap_ev_notifier;

  // RRC UE to NGAP adapter
  rrc_ue_ngap_adapter rrc_ue_ngap_ev_notifier;

  std::atomic<bool> amf_connected = {false};
};

} // namespace srs_cu_cp
} // namespace srsgnb
