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

#include "srsran/mac/bsr_config.h"
#include "srsran/mac/phr_config.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/sib/system_info_config.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/ran/tac.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/pucch_builder_params.h"
#include "srsran/scheduler/config/serving_cell_config.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"

namespace srsran {
namespace srs_du {

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

struct srs_builder_params {
  /// If present, defines the SRS period for SRS periodic resources, in slots.
  /// When not present, the SRS resources are set as aperiodic.
  std::optional<srs_periodicity> srs_period = std::nullopt;
  /// Maximum number of symbols per UL slot dedicated for SRS resources.
  /// \remark In case of Sounding Reference Signals (SRS) being used, the number of symbols should be reduced so that
  /// the PUCCH resources do not overlap in symbols with the SRS resources.
  /// \remark The SRS resources are always placed at the end of the slot.
  /// \remark As per TS 38.211, Section 6.4.1.4.1, SRS resource can only be placed in the last 6 symbols of a slot.
  bounded_integer<unsigned, 1, 6> max_nof_symbols = 2U;
  /// \c Transmission comb number, as per TS 38.211, Section 6.4.1.4.2, or TS 38.331, "SRS-Resource".
  tx_comb_size tx_comb = tx_comb_size::n4;
  /// Defines the number of symbols per SRS resource.
  srs_nof_symbols nof_symbols = srs_nof_symbols::n1;
  /// Defines the CS reuse factor for the SRS resources.
  /// \remark With 2 or 4 antenna ports, different cyclic shifts are used by the different antennas. This parameter
  /// defines how many UEs can be multiplexed in the same symbols and RBs by exploiting different cyclic shifts.
  /// Values: {no_cyclic_shift, two, three, four, six} for 2 UL antenna ports.
  /// Values: {no_cyclic_shift, three} for 4 UL antenna ports.
  nof_cyclic_shifts cyclic_shift_reuse_factor = nof_cyclic_shifts::no_cyclic_shift;
  /// Defines the reuse of the SRS sequence ID for different UEs within the same cell.
  /// \remark The goal of the SRS sequence ID would be to reduce the inter-cell interference. However, if the cell is
  /// not in a dense multi-cell environment, we can reuse different sequence ID for different cell UEs.
  /// Values: {1, 2, 3, 5, 6, 10, 15, 30}.
  unsigned sequence_id_reuse_factor = 1;
};

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

  /// Defines the maximum allowable channel delay in slots when runnning in NTN mode. see TS38.300 section 16.14.2.
  unsigned ntn_cs_koffset = 0;

  /// PUSCH Maximum of transmission layers. Limits the PUSCH maximum rank the UE is configrued with.
  unsigned pusch_max_nof_layers = 1;

  /// Whether contention-free random access is enabled for this cell.
  bool cfra_enabled = true;

  /// List of RAN slices to support in the scheduler.
  std::vector<slice_rrm_policy_config> rrm_policy_members;
};

} // namespace srs_du
} // namespace srsran
