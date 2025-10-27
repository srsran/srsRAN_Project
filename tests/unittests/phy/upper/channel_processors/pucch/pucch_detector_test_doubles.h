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

#include "../../../phy_test_utils.h"
#include "srsran/phy/upper/channel_processors/pucch/factories.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include <random>

namespace srsran {

class pucch_detector_spy : public pucch_detector
{
public:
  struct entry_format1 {
    const resource_grid_reader*                                   grid;
    const channel_estimate*                                       estimate;
    format1_configuration                                         config;
    pucch_format1_map<unsigned>                                   mux_nof_harq_ack;
    pucch_format1_map<pucch_detector::pucch_detection_result_csi> mux_results;
  };

  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                 const format0_configuration& config) override
  {
    return {};
  }

  const pucch_format1_map<pucch_detector::pucch_detection_result_csi>&
  detect(const resource_grid_reader&                  grid,
         const pucch_detector::format1_configuration& config,
         const pucch_format1_map<unsigned>&           mux_nof_harq_ack) override
  {
    entries_format1.emplace_back();
    entry_format1& entry   = entries_format1.back();
    entry.grid             = &grid;
    entry.config           = config;
    entry.mux_nof_harq_ack = mux_nof_harq_ack;

    for (const auto& this_pucch_nof_harq_ack : mux_nof_harq_ack) {
      unsigned ics  = this_pucch_nof_harq_ack.initial_cyclic_shift;
      unsigned occi = this_pucch_nof_harq_ack.time_domain_occ;

      pucch_uci_message message(
          {.nof_sr = 0, .nof_harq_ack = this_pucch_nof_harq_ack.value, .nof_csi_part1 = 0, .nof_csi_part2 = 0});

      // Generate random HARQ-ACK feedback bits
      span<uint8_t> harq_ack_bits = message.get_harq_ack_bits();
      std::generate(harq_ack_bits.begin(), harq_ack_bits.end(), [this]() { return rgen() & 1; });

      channel_state_information csi;
      csi.set_epre(0);
      csi.set_rsrp_dB(0);
      csi.set_sinr_dB(channel_state_information::sinr_type::channel_estimator, 0);
      csi.reset_time_alignment();

      entry.mux_results.insert(ics, occi, {{.uci_message = message, .detection_metric = 1}, csi});
    }

    return entries_format1.back().mux_results;
  }

  const std::vector<entry_format1>& get_entries_format1() const { return entries_format1; }

  void clear() { entries_format1.clear(); }

private:
  std::mt19937               rgen;
  std::vector<entry_format1> entries_format1;
};

PHY_SPY_FACTORY_TEMPLATE(pucch_detector);

} // namespace srsran
