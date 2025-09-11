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

#include "../../../lib/cu_cp/cu_cp_controller/amf_connection_manager.h"
#include "../e1ap/common/e1ap_cu_cp_test_messages.h"
#include "test_doubles/mock_amf.h"
#include "test_doubles/mock_cu_up.h"
#include "test_doubles/mock_du.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/plmn_identity.h"
#include <optional>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_test_amf_config {
  std::vector<supported_tracking_area> supported_tas;
  /// Injected AMF stub to handle CU-CP PDUs.
  std::unique_ptr<mock_amf> amf_stub;
};

static s_nssai_t               default_s_nssai{slice_service_type{1}, slice_differentiator{}};
static plmn_item               default_plmn_item{plmn_identity::test_value(), std::vector<s_nssai_t>{default_s_nssai}};
static supported_tracking_area default_supported_tracking_area{7, {default_plmn_item}};

struct cu_cp_test_env_params {
  cu_cp_test_env_params(
      unsigned                                                 max_nof_cu_ups_      = 8,
      unsigned                                                 max_nof_dus_         = 8,
      unsigned                                                 max_nof_ues_         = 8192,
      unsigned                                                 max_nof_drbs_per_ue_ = 8,
      const std::vector<std::vector<supported_tracking_area>>& amf_config_ = {{default_supported_tracking_area}},
      bool                                                     trigger_ho_from_measurements_ = true) :
    max_nof_cu_ups(max_nof_cu_ups_),
    max_nof_dus(max_nof_dus_),
    max_nof_ues(max_nof_ues_),
    max_nof_drbs_per_ue(max_nof_drbs_per_ue_),
    trigger_ho_from_measurements(trigger_ho_from_measurements_)
  {
    uint16_t amf_idx = 0;
    for (const auto& supported_tas : amf_config_) {
      amf_configs.emplace(amf_idx, cu_cp_test_amf_config{supported_tas, create_mock_amf()});
      amf_idx++;
    }
  }
  unsigned                                  max_nof_cu_ups;
  unsigned                                  max_nof_dus;
  unsigned                                  max_nof_ues;
  unsigned                                  max_nof_drbs_per_ue;
  std::map<unsigned, cu_cp_test_amf_config> amf_configs;
  bool                                      trigger_ho_from_measurements;
};

class cu_cp_test_environment
{
public:
  struct ue_context {
    rnti_t                                crnti = rnti_t::INVALID_RNTI;
    std::optional<gnb_du_ue_f1ap_id_t>    du_ue_id;
    std::optional<gnb_cu_ue_f1ap_id_t>    cu_ue_id;
    std::optional<ran_ue_id_t>            ran_ue_id;
    std::optional<amf_ue_id_t>            amf_ue_id;
    std::optional<gnb_cu_cp_ue_e1ap_id_t> cu_cp_e1ap_id;
    std::optional<gnb_cu_up_ue_e1ap_id_t> cu_up_e1ap_id;
  };

  explicit cu_cp_test_environment(cu_cp_test_env_params params = {});
  ~cu_cp_test_environment();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  cu_cp&      get_cu_cp() { return *cu_cp_inst; }
  mock_amf&   get_amf(size_t amf_index = 0) { return *amf_configs.at(amf_index).amf_stub; }
  mock_du&    get_du(size_t du_index) { return *dus.at(du_index); }
  mock_cu_up& get_cu_up(size_t cu_up_index) { return *cu_ups.at(cu_up_index); }

  /// Start CU-CP connection to AMF and run NG setup procedure to completion.
  void run_ng_setup();
  /// Drop TNL connection between the AMF and the CU-CP.
  bool drop_amf_connection(unsigned amf_idx);

  /// Establish a TNL connection between a DU and the CU-CP.
  std::optional<unsigned> connect_new_du();
  /// Drop TNL connection between a DU and the CU-CP.
  bool drop_du_connection(unsigned du_idx);
  /// Run F1 setup procedure to completion.
  bool run_f1_setup(unsigned                                         du_idx,
                    gnb_du_id_t                                      gnb_du_id = int_to_gnb_du_id(0x11),
                    std::vector<test_helpers::served_cell_item_info> cells = {test_helpers::served_cell_item_info{}});

  /// Establish a TNL connection between a CU-UP and the CU-CP.
  std::optional<unsigned> connect_new_cu_up();
  /// Drop TNL connection between a CU-UP and the CU-CP.
  bool drop_cu_up_connection(unsigned cu_up_idx);
  /// Run E1 setup procedure to completion
  bool run_e1_setup(unsigned cu_up_idx);

  /// Connect a new UE to CU-CP through a provided DU. It runs the full RRC setup procedure.
  [[nodiscard]] bool connect_new_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti);
  /// Runs the NAS Authentication for a given UE.
  [[nodiscard]] bool authenticate_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, amf_ue_id_t amf_ue_id);
  /// Runs the Security Mode procedure for a given UE.
  [[nodiscard]] bool setup_ue_security(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id);
  /// Finishes the registration for a given UE.
  [[nodiscard]] bool finish_ue_registration(unsigned du_idx, unsigned cu_up_idx, gnb_du_ue_f1ap_id_t du_ue_id);
  /// Requests PDU Session Resource Setup
  [[nodiscard]] bool
  request_pdu_session_resource_setup(unsigned du_idx, unsigned cu_up_idx, gnb_du_ue_f1ap_id_t du_ue_id);
  /// Runs PDU Session Resource Setup for a given UE.
  [[nodiscard]] bool
  setup_pdu_session(unsigned               du_idx,
                    unsigned               cu_up_idx,
                    gnb_du_ue_f1ap_id_t    du_ue_id,
                    rnti_t                 crnti,
                    gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
                    pdu_session_id_t       psi                          = pdu_session_id_t::min,
                    drb_id_t               drb_id                       = drb_id_t::drb1,
                    qos_flow_id_t          qfi                          = qos_flow_id_t::min,
                    byte_buffer            rrc_reconfiguration_complete = make_byte_buffer("00070e00cc6fcda5").value(),
                    bool                   is_initial_session           = true);
  /// Runs RRC setup, authentication, security, RRC Reconfiguration, PDU session setup for a given UE.
  [[nodiscard]] bool attach_ue(unsigned               du_idx,
                               unsigned               cu_up_idx,
                               gnb_du_ue_f1ap_id_t    du_ue_id,
                               rnti_t                 crnti,
                               amf_ue_id_t            amf_ue_id,
                               gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
                               pdu_session_id_t       psi               = pdu_session_id_t::min,
                               drb_id_t               drb_id            = drb_id_t::drb1,
                               qos_flow_id_t          qfi               = qos_flow_id_t::min,
                               byte_buffer rrc_reconfiguration_complete = make_byte_buffer("00070e00cc6fcda5").value());
  /// Reestablishes a UE connection, including RRC Reestablishment and RRC Reconfiguration procedures.
  /// \return True if the reestablishment was successful, false if RRC Setup/Reject was performed instead.
  [[nodiscard]] bool reestablish_ue(unsigned            du_idx,
                                    unsigned            cu_up_idx,
                                    gnb_du_ue_f1ap_id_t new_du_ue_id,
                                    rnti_t              new_crnti,
                                    rnti_t              old_crnti,
                                    pci_t               old_pci);

  /// Tick the CU-CP clock.
  void tick();

  /// Keep ticking the CU-CP clock until the condition provided returns true.
  bool
  tick_until(std::chrono::milliseconds timeout, const std::function<bool()>& stop_condition, bool real_time = true);

  /// Tick CU-CP timer until a NGAP PDU is sent.
  bool wait_for_ngap_tx_pdu(ngap_message&             ngap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{500},
                            unsigned                  amf_idx = 0);

  bool wait_for_e1ap_tx_pdu(unsigned                  cu_up_idx,
                            e1ap_message&             e1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{500});

  bool wait_for_f1ap_tx_pdu(unsigned                  du_idx,
                            f1ap_message&             f1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{500});

  const cu_cp_test_env_params& get_test_env_params() const { return params; }

  const ue_context* find_ue_context(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id) const;

  /// Get CU-CP configuration used to instantiate CU-CP.
  const cu_cp_configuration& get_cu_cp_cfg() const { return cu_cp_cfg; }

  // Helper functions
  [[nodiscard]] bool send_pdu_session_resource_setup_request_and_await_bearer_context_setup_request(
      const ngap_message& pdu_session_resource_setup_request,
      unsigned            du_idx,
      unsigned            cu_up_idx,
      gnb_du_ue_f1ap_id_t du_ue_id);

  [[nodiscard]] bool send_pdu_session_resource_setup_request_and_await_bearer_context_modification_request(
      const ngap_message& pdu_session_resource_setup_request,
      unsigned            cu_up_idx);

  [[nodiscard]] bool
  send_bearer_context_setup_response_and_await_ue_context_modification_request(unsigned               du_idx,
                                                                               unsigned               cu_up_idx,
                                                                               gnb_du_ue_f1ap_id_t    du_ue_id,
                                                                               gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
                                                                               pdu_session_id_t       psi,
                                                                               qos_flow_id_t          qfi);

  [[nodiscard]] bool
  send_bearer_context_modification_response_and_await_ue_context_modification_request(unsigned            du_idx,
                                                                                      unsigned            cu_up_idx,
                                                                                      gnb_du_ue_f1ap_id_t du_ue_id,
                                                                                      pdu_session_id_t    psi,
                                                                                      drb_id_t            drb_id,
                                                                                      qos_flow_id_t       qfi);

  [[nodiscard]] bool
  send_ue_context_modification_response_and_await_bearer_context_modification_request(unsigned            du_idx,
                                                                                      unsigned            cu_up_idx,
                                                                                      gnb_du_ue_f1ap_id_t du_ue_id,
                                                                                      rnti_t              crnti);

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_rrc_reconfiguration(
      unsigned                                           du_idx,
      unsigned                                           cu_up_idx,
      gnb_du_ue_f1ap_id_t                                du_ue_id,
      const std::map<pdu_session_id_t, drb_test_params>& pdu_sessions_to_add = {},
      const std::map<pdu_session_id_t, drb_id_t>& pdu_sessions_to_modify   = {{pdu_session_id_t::min, drb_id_t::drb1}},
      const std::optional<std::vector<srb_id_t>>& expected_srbs_to_add_mod = std::nullopt,
      const std::optional<std::vector<drb_id_t>>& expected_drbs_to_add_mod = std::nullopt,
      const std::vector<pdu_session_id_t>&        pdu_sessions_failed_to_modify = {});

  [[nodiscard]] bool send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response(
      unsigned                             du_idx,
      gnb_du_ue_f1ap_id_t                  du_ue_id,
      byte_buffer                          rrc_reconfiguration_complete,
      const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_setup,
      const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_setup);

  rrc_timers_t rrc_test_timer_values;

private:
  class worker_manager;

  cu_cp_test_env_params params;
  cu_cp_configuration   cu_cp_cfg{};

  /// Workers for CU-CP.
  std::unique_ptr<worker_manager> cu_cp_workers;

  /// Timers for CU-CP.
  timer_manager timers;

  /// Notifiers for the CU-CP interface.
  std::map<unsigned, cu_cp_test_amf_config> amf_configs;

  // emulated CU-UP nodes.
  std::unordered_map<unsigned, std::unique_ptr<mock_cu_up>> cu_ups;
  unsigned                                                  next_cu_up_idx = 0;

  // Emulated DU nodes.
  std::unordered_map<unsigned, std::unique_ptr<mock_du>> dus;
  unsigned                                               next_du_idx = 0;

  // Attached UEs.
  std::unordered_map<ran_ue_id_t, ue_context>                              attached_ues;
  std::map<unsigned, std::unordered_map<gnb_du_ue_f1ap_id_t, ran_ue_id_t>> du_ue_id_to_ran_ue_id_map;

  /// CU-CP instance.
  std::unique_ptr<cu_cp> cu_cp_inst;
};

} // namespace srs_cu_cp
} // namespace srsran
