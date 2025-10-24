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

#include "srsran/mac/bsr_config.h"
#include "srsran/mac/phr_config.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/sib/system_info_config.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/ran/ssb/ssb_configuration.h"
#include "srsran/ran/tac.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/pucch_builder_params.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"
#include "srsran/scheduler/config/srs_builder_params.h"

namespace srsran {
namespace srs_du {

/// Parameters that are used to initialize or build the \c PhysicalCellGroupConfig, TS 38.331.
struct phy_cell_group_params {
  /// \brief \c p-NR-FR1, part \c PhysicalCellGroupConfig, TS 38.331.
  /// The maximum total TX power to be used by the UE in this NR cell group across all serving cells in FR1.
  std::optional<bounded_integer<int, -30, 33>> p_nr_fr1;
};

/// Parameters that are used to generate UE DRX-Config.
struct drx_params {
  std::chrono::milliseconds on_duration;
  std::chrono::milliseconds long_cycle;
  std::chrono::milliseconds inactivity_timer;
  unsigned                  retx_timer_dl;
  unsigned                  retx_timer_ul;
};

/// Parameters that are used to initialize or build the \c MAC-CellGroupConfig, TS 38.331.
struct mac_cell_group_params {
  periodic_bsr_timer                            periodic_timer = periodic_bsr_timer::sf10;
  retx_bsr_timer                                retx_timer     = retx_bsr_timer::sf80;
  std::optional<logical_channel_sr_delay_timer> lc_sr_delay_timer;
  std::optional<sr_prohib_timer>                sr_prohibit_timer;
  sr_max_tx                                     max_tx           = sr_max_tx::n64;
  phr_prohibit_timer                            phr_prohib_timer = phr_prohibit_timer::sf10;
  std::optional<drx_params>                     drx;
};

/// Cell Configuration, including common and UE-dedicated configs, that the DU will use to generate other configs for
/// other layers (e.g. scheduler).
struct du_cell_config {
  pci_t               pci;
  tac_t               tac;
  nr_cell_global_id_t nr_cgi;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;

  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing scs_common;
  ssb_configuration  ssb_cfg;

  /// Whether the DU automatically attempts to activate the cell or waits for a command from the SMO.
  /// Note: If set to false, the DU won't add this cell to the list of served cells in the F1 Setup Request.
  bool enabled = true;

  dmrs_typeA_position dmrs_typeA_pos;

  /// CORESET#0 index of Table 13-{1, ..., 10}, TS 38.213.
  unsigned coreset0_idx;

  /// SearcSpace#0 index of Table 13-{11, ..., 15}, TS 38.213.
  unsigned searchspace0_idx;

  /// Parameters used to pack MIB.
  /// "cellBarred" as per MIB, TS 38.331. true = barred; false = notBarred.
  bool cell_barred;
  /// "intraFreqReselection" as per MIB, TS 38.331. true = allowed; false = notAllowed.
  bool intra_freq_resel;

  /// \c cellSelectionInfo, \c SIB1, as per TS 38.331.
  cell_selection_info cell_sel_info;

  /// \c cellAccessRelatedInfo, sent in \c SIB1, as per TS 38.331.
  cell_access_related_info cell_acc_rel_info;

  /// Content and scheduling information of SI-messages.
  std::optional<si_scheduling_info_config> si_config;

  /// \c ueTimersAndConstants, sent in \c SIB1, as per TS 38.331.
  ue_timers_and_constants_config ue_timers_and_constants;

  /// Cell-specific DL and UL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;

  /// UE-dedicated serving cell configuration.
  serving_cell_config ue_ded_serv_cell_cfg;

  /// Parameters to initialize/build the \c phy_cell_group.
  phy_cell_group_params pcg_params;

  /// Parameters to initialize/build the \c mac_cell_group_config.
  mac_cell_group_params mcg_params;

  /// Parameters for PUCCH-Config generation.
  pucch_builder_params pucch_cfg;

  /// Parameters for SRS-Config generation.
  srs_builder_params srs_cfg;

  /// Defines the maximum allowable channel delay in slots when running in NTN mode. see TS38.300 section 16.14.2.
  unsigned ntn_cs_koffset = 0;

  /// Whether DL HARQ Mode B is enabled for this NTN cell.
  bool dl_harq_mode_b = false;

  /// Whether UL HARQ Mode B is enabled for this NTN cell.
  bool ul_harq_mode_b = false;

  /// PUSCH Maximum of transmission layers. Limits the PUSCH maximum rank the UE is configured with.
  unsigned pusch_max_nof_layers = 1;

  /// Whether contention-free random access is enabled for this cell.
  bool cfra_enabled = true;

  /// List of RAN slices to support in the scheduler.
  std::vector<slice_rrm_policy_config> rrm_policy_members;
};

} // namespace srs_du
} // namespace srsran
