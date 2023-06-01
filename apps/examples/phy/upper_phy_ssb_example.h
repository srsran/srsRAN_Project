/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsran/phy/upper/upper_phy_timing_handler.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_mapping.h"
#include <memory>
#include <string>

namespace srsran {

/// \brief Upper PHY processor application example interface.
///
/// It shows how to use the upper PHY processor interfaces. It implements the upper PHY timing interface.
class upper_phy_ssb_example : public upper_phy_timing_handler
{
public:
  /// \brief Waits for the TTI boundary.
  ///
  /// Blocks the thread execution upon the notification of a TTI boundary. This is used for throttling the execution in
  /// a TTI basis from an external thread.
  virtual void wait_tti_boundary() = 0;

  /// Stops the upper PHY execution.
  virtual void stop() = 0;

  /// Collects the necessary parameters to generate a periodic DL waveform.
  struct ssb_configuration {
    /// Physical cell identifier.
    pci_t phys_cell_id;
    /// Cyclic prefix.
    cyclic_prefix cp;
    /// SSB period in milliseconds.
    unsigned period_ms;
    /// PSS scaling relative to SSS, in decibels.
    float beta_pss_dB;
    /// SSB candidates to transmit.
    std::vector<unsigned> ssb_idx;
    /// Maximum number of SSB candidates.
    unsigned L_max;
    /// SSB offset between the common resource grid and the beginning of the SSB as a number of 15kHz subcarriers.
    ssb_subcarrier_offset subcarrier_offset;
    /// SSB offset between Point A and the beginning of the common resource grid in RB.
    ssb_offset_to_pointA offset_pointA;
    /// SS/PBCH pattern case (A,B,C,D,E).
    ssb_pattern_case pattern_case;
  };

  /// Collects upper PHY sample configuration parameters.
  struct configuration {
    /// General upper PHY logging level.
    std::string log_level;
    /// Specifies the maximum number of PRBs.
    unsigned max_nof_prb;
    /// Specifies the maximum number of antenna ports.
    unsigned max_nof_ports;
    /// Specifies the resource grid pool size.
    unsigned rg_pool_size;
    /// Scheduling sample configuration.
    ssb_configuration ssb_config;
    /// Specifies the LDPC encoder type.
    std::string ldpc_encoder_type;
    /// Resource grid gateway.
    upper_phy_rg_gateway* gateway;
    /// Uplink request processor.
    upper_phy_rx_symbol_request_notifier* rx_symb_req_notifier;
    /// Enable the generation and allocation of pseudo-random data within the resource grid.
    bool enable_random_data;
    /// Enable uplink processing.
    bool enable_ul_processing;
    /// Enable PRACH processing.
    bool enable_prach_processing;
    /// Modulation scheme used for the pseudo-random data.
    modulation_scheme data_modulation;
  };

  /// Creates an upper PHY sample.
  static std::unique_ptr<upper_phy_ssb_example> create(const configuration& config);
};

} // namespace srsran
