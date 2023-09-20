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

#include "../cell/cell_configuration.h"
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
  void handle_dl_ack_info(mac_harq_ack_report_status ack_value, sch_mcs_index used_mcs, pdsch_mcs_table mcs_table);

  /// \brief Update UL link quality with the latest UL CRC feedback.
  void handle_ul_crc_info(bool crc, sch_mcs_index used_mcs, pusch_mcs_table mcs_table);

  /// \brief Derives an adequate MCS given the reported CQI and experienced BLER.
  optional<sch_mcs_index> calculate_dl_mcs(pdsch_mcs_table mcs_table) const;

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

private:
  /// \brief Get the effective CQI, accounting CQI and OLLA offset, to be used for MCS derivation.
  float get_effective_cqi() const;

  /// \brief Get the value of UL SNR after applying the link adaptation SNR offset.
  float get_effective_snr() const;

  const cell_configuration&       cell_cfg;
  const ue_channel_state_manager& ue_ch_st;

  optional<olla_algorithm> dl_olla;
  optional<olla_algorithm> ul_olla;
};

} // namespace srsran
