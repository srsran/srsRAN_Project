/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

  /// Connect a new UE to CU-CP through a provided DU.
  bool connect_new_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti);

  /// Tick the CU-CP clock.
  void tick();

  /// Keep ticking the CU-CP clock until the condition provided returns true.
  bool tick_until(std::chrono::milliseconds timeout, const std::function<bool()>& stop_condition);

  /// Tick CU-CP timer until a NGAP PDU is sent.
  bool wait_for_ngap_tx_pdu(ngap_message&             ngap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{1000});

  bool wait_for_e1ap_tx_pdu(unsigned                  cu_up_idx,
                            e1ap_message&             e1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{1000});

  bool wait_for_f1ap_tx_pdu(unsigned                  du_idx,
                            f1ap_message&             f1ap_pdu,
                            std::chrono::milliseconds timeout = std::chrono::milliseconds{1000});

  const cu_cp_test_env_params& get_test_env_params() const { return params; }

private:
  class worker_manager;
  class gateway_manager;

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

  /// CU-CP instance.
  std::unique_ptr<cu_cp> cu_cp_inst;
};

} // namespace srs_cu_cp
} // namespace srsran
