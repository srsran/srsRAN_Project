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

#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <string>
#include <vector>

namespace srsgnb {

/// RF cell driver configuration.
struct rf_driver_appconfig {
  /// Sampling frequency in MHz.
  double srate_MHz = 61.44;
  /// RF driver name.
  std::string device_driver = "zmq";
  /// RF driver arguments.
  std::string device_args = "tx_port=tcp://*:5000,rx_port=tcp://localhost:6000";
  /// Transmission gain.
  double tx_gain = 60.0;
  /// Reception gain.
  double rx_gain = 70.0;
  /// Number of ports.
  unsigned nof_ports = 1;
};

/// RF cell configuration.
struct rf_cell {
  /// Downlink arfcn.
  unsigned dl_arfcn = 536020;
  /// Subcarrier spacing common.
  subcarrier_spacing scs_common = subcarrier_spacing::kHz15;
  /// Channel bandwidth in MHz.
  // :TODO: convert this to a variant to support FR2.
  bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
  // :TODO: can we derive these parameters?
  unsigned offset_to_point_a = 40;
  unsigned k_ssb             = 6;
};

/// Cell configuration
struct cell_appconfig {
  /// Rf parameters.
  rf_cell rf;
  /// Physical cell identifier.
  unsigned pci = 1;
};

struct amf_appconfig {
  std::string amf_addr;
  uint16_t    amf_port = 38412;
  std::string amf_bind_addr;
};

struct cu_cp_appconfig {
  /// Application identifier.
  int gnb_id = 411;
  /// Node name.
  std::string ran_node_name = "srsgnb01";
  /// PLMN id
  std::string plmn_id = "00f110";
  /// TAC.
  int           tac = 7;
  amf_appconfig amf_cfg;
};

/// Monolithic gnb application configuration.
struct gnb_appconfig {
  /// Add one cell by default.
  std::vector<cell_appconfig> cells_cfg = {{}};
  cu_cp_appconfig             cu_cfg;
  rf_driver_appconfig         rf_driver_cfg;
  /// Debug logger flag.
  bool debug_logger = false;
};

} // namespace srsgnb
