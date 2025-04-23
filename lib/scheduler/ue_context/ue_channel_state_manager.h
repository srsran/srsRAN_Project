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

#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/logical_channel/phr_report.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/pusch/pusch_tpmi_select.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/math/exponential_averager.h"

namespace srsran {

/// \brief This classes manages all the information related with the channel state that has been received from the UE
/// via CSI (e.g. CQI) or via gNB PHY measurements (e.g. UL SINR).
class ue_channel_state_manager
{
public:
  ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg_, unsigned nof_dl_ports_);

  const std::optional<csi_report_data>& get_latest_csi_report() const { return latest_csi_report; }

  const std::optional<pusch_tpmi_select_info>& get_latest_tpmi_select_info() const
  {
    return last_pusch_tpmi_select_info;
  }

  void update_pusch_snr(float snr_db);

  /// \brief Get PUSCH SNR in dB.
  float get_pusch_snr() const { return pusch_snr_db; }

  /// \brief Get average PUSCH SNR in dB.
  float get_pusch_average_sinr() const { return average_pusch_sinr_dB.get_average_value(); }

  csi_report_wideband_cqi_type get_wideband_cqi() const { return wideband_cqi; }

  /// \brief Gets the number of recommended layers to be used in DL based on reported RI.
  unsigned get_nof_dl_layers() const { return recommended_dl_layers; }

  /// \brief Gets the number of recommended layers to be used in PUSCH based on the reported channel coefficients.
  unsigned get_nof_ul_layers() const;

  /// Gets the most suitable TPMI for a given number of layers.
  unsigned get_recommended_pusch_tpmi(unsigned nof_layers) const;

  /// \brief Fetches the precoding codebook to be used in DL based on reported PMI and the chosen nof layers.
  std::optional<pdsch_precoding_info> get_precoding(unsigned chosen_nof_layers, unsigned nof_rbs) const
  {
    srsran_assert(chosen_nof_layers <= nof_dl_ports, "Invalid number of layers chosen");
    std::optional<pdsch_precoding_info> precoding_info;
    if (nof_dl_ports <= 1) {
      // In case of 1 DL port, no precoding is used.
      return precoding_info;
    }
    precoding_info.emplace();
    precoding_info->nof_rbs_per_prg = nof_rbs;
    precoding_info->prg_infos.emplace_back(recommended_prg_info[nof_layers_to_index(chosen_nof_layers)]);
    return precoding_info;
  }

  /// Update UE with the latest CSI report for a given cell.
  bool handle_csi_report(const csi_report_data& csi_report);

  /// Update UE with the latest Sounding Reference Signal (SRS) channel matrix.
  void update_srs_channel_matrix(const srs_channel_matrix& channel_matrix, tx_scheme_codebook codebook_cfg);

private:
  /// \brief Number of indexes -> nof_layers for precoding (Options: 1, 2, 3, 4 layers).
  static constexpr size_t NOF_LAYER_CHOICES = 4;
  /// Defines the alpha value for the exponential moving average of the PUSCH SINR.
  static constexpr float alpha_ema_sinr = 0.5f;

  /// Mapping of number of layers to array index.
  static size_t nof_layers_to_index(unsigned nof_layers) { return nof_layers - 1U; }

  /// \brief Number of DL ports.
  unsigned nof_dl_ports;

  /// Estimated PUSCH SNR, in dB.
  float pusch_snr_db;

  /// Time-averaged PUSCH SINR, in dB.
  exp_average_fast_start<float> average_pusch_sinr_dB;

  /// \brief Recommended CQI to be used to derive the DL MCS.
  csi_report_wideband_cqi_type wideband_cqi;

  /// \brief Recommended nof layers based on reports.
  unsigned recommended_dl_layers = 1;

  /// \brief List of Recommended PMIs for different number of active layers. Position n is for layer n+1.
  static_vector<pdsch_precoding_info::prg_info, NOF_LAYER_CHOICES> recommended_prg_info;

  /// Latest CSI report received from the UE.
  std::optional<csi_report_data> latest_csi_report;

  /// \brief Latest PUSCH Transmit Precoding Matrix Indication (TPMI) information.
  ///
  /// The TPMI selection information is calculated assuming a noise variance equal to the square of channel coefficients
  /// matrix Frobenius norm. See \ref get_pusch_tpmi_select_info for more information.
  ///
  /// Set to \c std::nullopt if no SRS channel coefficients have been reported.
  std::optional<pusch_tpmi_select_info> last_pusch_tpmi_select_info;
};

} // namespace srsran
