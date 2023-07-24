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

#include "srsran/mac/phr_report.h"
#include "srsran/ran/csi_report/csi_report_wideband_cqi.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

/// \brief This classes manages all the information related with the channel state that has been received from the UE
/// via CSI (e.g. CQI) or via gNB PHY measurements (e.g. UL SINR).
class ue_channel_state_manager
{
public:
  ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg_, unsigned nof_dl_ports_);

  const optional<csi_report_data>& get_latest_csi_report() const { return latest_csi_report; }

  void update_pusch_snr(float snr_db) { pusch_snr_db = snr_db; }

  /// \brief Get PUSCH SNR in dBs.
  float get_pusch_snr() const { return pusch_snr_db; }

  csi_report_wideband_cqi_type get_wideband_cqi() const { return wideband_cqi; }

  /// \brief Gets the number of recommended layers to be used in DL based on reported RI.
  unsigned get_nof_dl_layers() const { return recommended_dl_layers; }

  /// \brief Fetches the number of recommended layers to be used in UL.
  unsigned get_nof_ul_layers() const { return 1; }

  /// \brief Fetches the precoding codebook to be used in DL based on reported PMI and the chosen nof layers.
  optional<pdsch_precoding_info> get_precoding(unsigned chosen_nof_layers, prb_interval pdsch_prbs) const
  {
    srsran_assert(chosen_nof_layers <= nof_dl_ports, "Invalid number of layers chosen");
    optional<pdsch_precoding_info> precoding_info;
    if (nof_dl_ports <= 1) {
      // In case of 1 DL port, no precoding is used.
      return precoding_info;
    }
    precoding_info.emplace();
    precoding_info->nof_rbs_per_prg = pdsch_prbs.length();
    precoding_info->prg_infos.emplace_back(recommended_prg_info[nof_layers_to_index(chosen_nof_layers)]);
    return precoding_info;
  }

  /// Update UE with the latest CSI report for a given cell.
  bool handle_csi_report(const csi_report_data& csi_report);

  /// Update UE with the latest PHR for a given cell.
  void handle_phr(const cell_ph_report& phr) { latest_phr = phr; }

private:
  /// \brief Number of indexes -> nof_layers for precoding (Options: 1 layer, 2 layers, 4 layers).
  static constexpr size_t NOF_LAYER_CHOICES = 3;

  /// Mapping of number of layers to array index.
  static size_t nof_layers_to_index(unsigned nof_layers) { return nof_layers >> 1U; }

  /// \brief Number of DL ports.
  unsigned nof_dl_ports;

  /// Estimated PUSCH SNR, in dB.
  float pusch_snr_db;

  /// \brief Recommended CQI to be used to derive the DL MCS.
  csi_report_wideband_cqi_type wideband_cqi;

  /// \brief Recommended nof layers based on reports.
  unsigned recommended_dl_layers = 1;

  /// \brief List of Recommended PMIs for different number of active layers. Position 0 is for 1 layer, position 1 is
  /// for 2 layers and position 3 for 4 layers.
  static_vector<pdsch_precoding_info::prg_info, NOF_LAYER_CHOICES> recommended_prg_info;

  /// Latest CSI report received from the UE.
  optional<csi_report_data> latest_csi_report;

  /// Latest PHR received from the UE.
  optional<cell_ph_report> latest_phr;
};

} // namespace srsran
