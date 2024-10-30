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

#include "srsran/adt/circular_array.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/logical_channel/phr_report.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/pusch/pusch_tpmi_select.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// \brief This classes manages all the information related with the channel state that has been received from the UE
/// via CSI (e.g. CQI) or via gNB PHY measurements (e.g. UL SINR).
class ue_channel_state_manager
{
public:
  ue_channel_state_manager(const scheduler_ue_expert_config& expert_cfg_, unsigned nof_dl_ports_);

  const std::optional<csi_report_data>& get_latest_csi_report() const { return latest_csi_report; }

  void update_pusch_snr(float snr_db) { pusch_snr_db = snr_db; }

  /// \brief Get PUSCH SNR in dB.
  float get_pusch_snr() const { return pusch_snr_db; }

  csi_report_wideband_cqi_type get_wideband_cqi() const { return wideband_cqi; }

  /// \brief Gets the number of recommended layers to be used in DL based on reported RI.
  unsigned get_nof_dl_layers() const { return recommended_dl_layers; }

  /// \brief Gets the number of recommended layers to be used in PUSCH based on the reported channel coefficients.
  unsigned get_nof_ul_layers() const;

  /// Gets the most suitable TPMI for a given number of layers.
  unsigned get_recommended_pusch_tpmi(unsigned nof_layers) const;

  /// \brief Fetches the precoding codebook to be used in DL based on reported PMI and the chosen nof layers.
  std::optional<pdsch_precoding_info> get_precoding(unsigned chosen_nof_layers, prb_interval pdsch_prbs) const
  {
    srsran_assert(chosen_nof_layers <= nof_dl_ports, "Invalid number of layers chosen");
    std::optional<pdsch_precoding_info> precoding_info;
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

  /// Update UE with the latest Sounding Reference Signal (SRS) channel matrix.
  void update_srs_channel_matrix(const srs_channel_matrix& channel_matrix, tx_scheme_codebook_subset cb_subset);

  /// Update UE with the latest PHR for a given cell.
  void handle_phr(const cell_ph_report& phr);

  /// Save the number of PUSCH PRBs allocated for a given slot.
  void save_pusch_nof_prbs(slot_point slot, unsigned nof_prbs);

  /// Adapt the number of PUSCH PRBs to the PHR.
  unsigned adapt_pusch_prbs_to_phr(unsigned nof_prbs) const;

private:
  /// \brief Number of indexes -> nof_layers for precoding (Options: 1, 2, 3, 4 layers).
  static constexpr size_t NOF_LAYER_CHOICES = 4;

  /// This variable defines how many PUSCH PRB allocations needs to be stored in the internal ring-buffer; it is the
  /// maximum expected delay (in slots) between the for which slot the PUSCH is scheduled and the slot at which the PHR
  /// is received; this delay depends on the PHY processing capabilities. For simplicity, we take round the number to a
  /// multiple of 10.
  static constexpr size_t MAX_PHR_IND_DELAY_SLOTS = 40;

  /// Mapping of number of layers to array index.
  static size_t nof_layers_to_index(unsigned nof_layers) { return nof_layers - 1U; }

  /// \brief Number of DL ports.
  unsigned nof_dl_ports;

  /// Estimated PUSCH SNR, in dB.
  float pusch_snr_db;

  /// \brief Recommended CQI to be used to derive the DL MCS.
  csi_report_wideband_cqi_type wideband_cqi;

  /// \brief Recommended nof layers based on reports.
  unsigned recommended_dl_layers = 1;

  /// \brief List of Recommended PMIs for different number of active layers. Position n is for layer n+1.
  static_vector<pdsch_precoding_info::prg_info, NOF_LAYER_CHOICES> recommended_prg_info;

  /// Latest CSI report received from the UE.
  std::optional<csi_report_data> latest_csi_report;

  /// \brief Latest PHR received from the UE.
  struct ue_phr_report {
    cell_ph_report       phr;
    std::optional<float> ph_times_rbs;
  };

  /// Latest PHR received from the UE.
  std::optional<ue_phr_report> latest_phr;

  /// \brief Latest PUSCH Transmit Precoding Matrix Indication (TPMI) information.
  ///
  /// The TPMI selection information is calculated assuming a noise variance equal to the square of channel coefficients
  /// matrix Frobenius norm. See \ref get_pusch_tpmi_select_info for more information.
  ///
  /// Set to \c std::nullopt if no SRS channel coefficients have been reported.
  std::optional<pusch_tpmi_select_info> last_pusch_tpmi_select_info;

  /// \brief Entry for PUSCH number of PRBs allocation.
  struct pusch_prbs_entry {
    slot_point slot_rx;
    unsigned   nof_prbs;
  };

  /// \brief Ring of PUSCH number of PRBs allocation indexed by slot.
  circular_array<pusch_prbs_entry, get_allocator_ring_size_gt_min(MAX_PHR_IND_DELAY_SLOTS)> pusch_nof_prbs_grid;

  srslog::basic_logger& logger;
};

} // namespace srsran
