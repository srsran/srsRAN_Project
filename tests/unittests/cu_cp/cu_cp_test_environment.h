/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "test_doubles/mock_amf.h"
#include "test_doubles/mock_cu_up.h"
#include "test_doubles/mock_du.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/ngap/ngap_configuration_helpers.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct cu_cp_test_env_params {
  unsigned max_nof_cu_ups = 4;
  unsigned max_nof_dus    = 4;
  /// Injected AMF stub to handle CU-CP PDUs.
  std::unique_ptr<mock_amf> amf_stub = create_mock_amf();
};

class cu_cp_test_environment
{
public:
  struct ue_context {
    rnti_t                        crnti = rnti_t::INVALID_RNTI;
    optional<gnb_du_ue_f1ap_id_t> du_ue_id;
    optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
    optional<ran_ue_id_t>         ran_ue_id;
    optional<amf_ue_id_t>         amf_ue_id;
  };

  explicit cu_cp_test_environment(cu_cp_test_env_params params = {});
  ~cu_cp_test_environment();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  cu_cp&      get_cu_cp() { return *cu_cp_inst; }
  mock_amf&   get_amf() { return *amf_stub; }
  mock_du&    get_du(size_t du_index) { return *dus.at(du_index); }
  mock_cu_up& get_cu_up(size_t cu_up_index) { return *cu_ups.at(cu_up_index); }

  /// Start CU-CP connection to AMF and run NG setup procedure to completion.
  void run_ng_setup();

  /// Establish a TNL connection between a DU and the CU-CP.
  optional<unsigned> connect_new_du();
  /// Drop TNL connection between a DU and the CU-CP.
  bool drop_du_connection(unsigned du_idx);
  /// Run F1 setup procedure to completion.
  bool run_f1_setup(unsigned du_idx);

  /// Establish a TNL connection between a CU-UP and the CU-CP.
  optional<unsigned> connect_new_cu_up();
  /// Drop TNL connection between a CU-UP and the CU-CP.
  bool drop_cu_up_connection(unsigned cu_up_idx);
  /// Run E1 setup procedure to completion
  bool run_e1_setup(unsigned cu_up_idx);

  /// Connect a new UE to CU-CP through a provided DU. It runs the full RRC setup procedure.
  bool connect_new_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti);
  /// Runs the NAS Authentication for a given UE.
  bool authenticate_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, amf_ue_id_t amf_ue_id);
  /// Runs the Security Mode procedure for a given UE.
  bool setup_ue_security(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id);
  /// Runs RRC setup, authentication, security, RRC Reconfiguration for a given UE.
  bool attach_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti, amf_ue_id_t amf_ue_id);

  bool
  reestablish_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t new_du_ue_id, rnti_t new_crnti, rnti_t old_crnti, pci_t old_pci);

  /// Tick the CU-CP clock.
  void tick();

  /// Keep ticking the CU-CP clock until the condition provided returns true.
  bool tick_until(std::chrono::milliseconds timeout, const std::function<bool()>& stop_condition);

  /// Tick CU-CP timer until a NGAP PDU is sent.
  bool wait_for_ngap_tx_pdu(ngap_message& ngap_pdu, std::chrono::milliseconds timeout = std::chrono::milliseconds{500});

  bool wait_for_e1ap_tx_pdu(unsigned                  cu_up_idx,
                            e1ap_message&             e1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{500});

  bool wait_for_f1ap_tx_pdu(unsigned                  du_idx,
                            f1ap_message&             f1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{500});

  const cu_cp_test_env_params& get_test_env_params() const { return params; }

  const ue_context* find_ue_context(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id) const;

private:
  class worker_manager;

  cu_cp_test_env_params params;

  /// Workers for CU-CP.
  std::unique_ptr<worker_manager> cu_cp_workers;

  /// Timers for CU-CP.
  timer_manager timers;

  /// Notifiers for the CU-CP interface.
  std::unique_ptr<mock_amf> amf_stub;

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
