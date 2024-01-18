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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/ngap/ngap_configuration_helpers.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_test_environment
{
public:
  cu_cp_test_environment(const ngap_configuration& ngap_cfg = config_helpers::make_default_ngap_config());
  ~cu_cp_test_environment();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  /// Tick the CU-CP clock.
  void tick();

  /// Keep ticking the CU-CP clock until the condition provided returns true.
  bool tick_until(std::chrono::milliseconds timeout, const std::function<bool()>& stop_condition);

  /// Try popping the oldest NGAP PDU sent by the CU-CP.
  bool try_pop_ngap_tx_pdu(ngap_message& ngap_pdu);

  /// Tick CU-CP timer until a NGAP PDU is sent.
  bool wait_for_ngap_tx_pdu(ngap_message& ngap_pdu, std::chrono::milliseconds timeout);

  /// Try popping the oldest E1AP PDU sent by the CU-CP.
  bool try_pop_e1ap_tx_pdu(e1ap_message& e1ap_pdu);

  bool wait_for_e1ap_tx_pdu(e1ap_message& e1ap_pdu, std::chrono::milliseconds timeout);

private:
  class worker_manager;
  class gateway_manager;

  /// Workers for CU-CP.
  std::unique_ptr<worker_manager> cu_cp_workers;

  /// Timers for CU-CP.
  timer_manager timers;

  /// Notifiers for the CU-CP interface.
  std::unique_ptr<gateway_manager> gw;

  /// CU-CP instance.
  std::unique_ptr<cu_cp_interface> cu_cp;
};

} // namespace srs_cu_cp
} // namespace srsran