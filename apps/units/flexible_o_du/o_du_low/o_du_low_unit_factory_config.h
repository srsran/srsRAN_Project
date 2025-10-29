/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_low_hal_factory.h"
#include "srsran/du/du_low/du_low_executor_mapper.h"
#include "srsran/du/du_low/o_du_low_config.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {

struct du_low_unit_config;

/// O-RAN DU low unit configuration.
struct o_du_low_unit_config {
  struct du_low_config {
    srs_du::cell_prach_ports_entry         prach_ports;
    unsigned                               max_puschs_per_slot;
    frequency_range                        freq_range;
    duplex_mode                            duplex;
    subcarrier_spacing                     scs_common;
    unsigned                               bw_rb;
    std::optional<tdd_ul_dl_config_common> tdd_pattern;
    unsigned                               prach_config_index;
    unsigned                               pusch_max_nof_layers;
    unsigned                               nof_rx_antennas;
    unsigned                               nof_tx_antennas;
  };

  const du_low_unit_config&                      du_low_unit_cfg;
  std::vector<du_low_config>                     cells;
  fapi_adaptor::phy_fapi_fastpath_adaptor_config fapi_cfg;
};

/// O-RAN DU low unit dependencies.
struct o_du_low_unit_dependencies {
  upper_phy_rg_gateway&                 rg_gateway;
  upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier;
  srs_du::du_low_executor_mapper&       workers;
};

} // namespace srsran
