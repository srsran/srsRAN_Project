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

#include "../config/cell_configuration.h"
#include "../support/outer_loop_link_adaptation.h"
#include "ue_channel_state_manager.h"

namespace srsran {

/// \brief Component that handles the derivation of the MCS for a UE, given its CSI, PHY estimates, and experienced
/// channel BLER.
class ue_link_adaptation_controller
{
public:
  ue_link_adaptation_controller(const cell_configuration& cell_cfg_, const ue_channel_state_manager& ue_channel_state);

  /// \brief Update DL link quality with the latest DL HARQ feedback.
  void handle_dl_ack_info(bool                         ack,
                          sch_mcs_index                used_mcs,
                          pdsch_mcs_table              mcs_table,
                          std::optional<sch_mcs_index> olla_mcs);

  /// \brief Update UL link quality with the latest UL CRC feedback.
  void handle_ul_crc_info(bool                         crc,
                          sch_mcs_index                used_mcs,
                          pusch_mcs_table              mcs_table,
                          std::optional<sch_mcs_index> olla_mcs);

  /// \brief Derives an adequate MCS given the reported CQI and experienced BLER.
  std::optional<sch_mcs_index> calculate_dl_mcs(pdsch_mcs_table mcs_table) const;

  /// \brief Derives an adequate MCS given the estimated UL SNR and experienced BLER.
  sch_mcs_index calculate_ul_mcs(pusch_mcs_table mcs_table) const;

  /// \brief Get the value of the DL CQI offset that the OLLA algorithm is currently using.
  float dl_cqi_offset() const { return dl_olla.has_value() ? dl_olla->offset_db() : 0.0f; }

  /// \brief Get the value of the UL SNR offset that the OLLA algorithm is currently using.
  float ul_snr_offset_db() const { return ul_olla.has_value() ? ul_olla->offset_db() : 0.0f; }

  /// Checks whether OLLA is enabled for DL.
  bool is_dl_olla_enabled() const { return dl_olla.has_value(); }

  /// Checks whether OLLA is enabled for UL.
  bool is_ul_olla_enabled() const { return ul_olla.has_value(); }

  /// \brief Get the effective CQI, accounting CQI and OLLA offset, to be used for MCS derivation.
  float get_effective_cqi() const;

  /// \brief Get the wideband CQI.
  unsigned get_wideband_cqi() const { return ue_ch_st.get_wideband_cqi().value(); }

private:
  /// \brief Get the value of UL SNR after applying the link adaptation SNR offset.
  float get_effective_snr() const;

  /// \brief Update the DL MCS boundaries based on the chosen MCS table.
  void update_dl_mcs_lims(pdsch_mcs_table mcs_table);

  /// \brief Update the UL MCS boundaries based on the chosen MCS table.
  void update_ul_mcs_lims(pusch_mcs_table mcs_table, bool transform_precoder);

  const cell_configuration&       cell_cfg;
  const ue_channel_state_manager& ue_ch_st;

  // Limits for the MCS selection.
  pdsch_mcs_table               last_dl_mcs_table;
  interval<sch_mcs_index, true> dl_mcs_lims;
  pusch_mcs_table               last_ul_mcs_table;
  bool                          last_transform_precoder;
  interval<sch_mcs_index, true> ul_mcs_lims;

  std::optional<olla_algorithm> dl_olla;
  std::optional<olla_algorithm> ul_olla;
};

} // namespace srsran
