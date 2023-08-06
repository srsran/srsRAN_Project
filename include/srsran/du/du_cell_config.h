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

#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

inline unsigned format1_cp_step_to_uint(nof_cyclic_shifts opt)
{
  return static_cast<unsigned>(opt);
}

/// Collects the parameters for PUCCH Format 1 that can be configured.
struct pucch_f1_params {
  /// Number of possible Initial Cyclic Shifts, equally spaced within the range {0,...,11}, as per \c PUCCH-format1, in
  /// \c PUCCH-Config, TS 38.331.
  nof_cyclic_shifts nof_cyc_shifts{nof_cyclic_shifts::no_cyclic_shift};
  /// Indicates whether OCCs (as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331) are supported.
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> nof_symbols{14};
  bool                             intraslot_freq_hopping{false};
};

/// Collects the parameters for PUCCH Format 2 that can be configured.
struct pucch_f2_params {
  bounded_integer<unsigned, 1, 2> nof_symbols{2};
  unsigned                        max_nof_rbs{1};
  /// Maximum payload in bits that can be carried by PUCCH Format 2. When this field is set, \c max_nof_rbs is ignored
  /// and the maximum number of RBs is computed according to \ref get_pucch_format2_max_nof_prbs.
  optional<unsigned>  max_payload_bits;
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};
  /// For intraslot-freq-hopping, \c nof_symbols must be set to 2.
  bool intraslot_freq_hopping{false};
};

/// \brief Parameters for PUCCH configuration.
/// Defines the parameters that are used for the PUCCH configuration builder. These parameters are used to define the
/// number of PUCCH resources, as well as the PUCCH format-specific parameters.
struct pucch_builder_params {
  /// UE specific parameters. Use to set the number of resources per UE for HARQ-ACK reporting (not including SR/CSI
  /// dedicated resources). NOTE: by default, each UE is assigned 1 SR and 1 CSI resource.
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f1_res_harq = 3;
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f2_res_harq = 6;
  /// \brief Number of separate PUCCH resource sets for HARQ-ACK reporting that are available in a cell.
  /// \remark UEs will be distributed possibly over different HARQ-ACK PUCCH sets; the more sets, the fewer UEs will
  /// have to share the same set, which reduces the chances that UEs won't be allocated PUCCH due to lack of resources.
  /// However, the usage of PUCCH-dedicated REs will be proportional to the number of sets.
  unsigned nof_cell_harq_pucch_res_sets = 1;
  /// Defines how many PUCCH F1 resources should be dedicated for SR at cell level; each UE will be allocated 1 resource
  /// for SR.
  unsigned nof_sr_resources = 2;
  /// Defines how many PUCCH F2 resources should be dedicated for CSI at cell level; each UE will be allocated 1
  /// resource for CSI.
  unsigned nof_csi_resources = 1;

  /// PUCCH Format specific parameters.
  pucch_f1_params f1_params;
  pucch_f2_params f2_params;
};

struct cell_selection_info {
  /// \brief \c q-RxLevMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dBm.
  /// Indicates the required minimum received RSRP level for cell selection/re-selection (see \c Q-RxLevMin, TS 38.311).
  bounded_integer<int, -70, -22> q_rx_lev_min = -70;
  /// \brief \c q-QualMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dB.
  /// Indicates the required minimum received RSRQ level for cell selection/re-selection (see \c Q-QualMin, TS 38.311).
  bounded_integer<int, -43, -12> q_qual_min = -20;
};

/// Parameters that are used to initialize or build the \c PhysicalCellGroupConfig, TS 38.331.
struct phy_cell_group_params {
  /// \brief \c p-NR-FR1, part \c PhysicalCellGroupConfig, TS 38.331.
  /// The maximum total TX power to be used by the UE in this NR cell group across all serving cells in FR1.
  optional<bounded_integer<int, -30, 33>> p_nr_fr1;
};

/// Cell Configuration, including common and UE-dedicated configs, that the DU will use to generate other configs for
/// other layers (e.g. scheduler).
struct du_cell_config {
  pci_t               pci;
  uint32_t            tac;
  nr_cell_global_id_t nr_cgi;

  /// \c cellSelectionInfo, \c SIB1, as per TS 38.311.
  cell_selection_info cell_sel_info;

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

  /// Cell-specific DL and UL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;

  /// UE-dedicated serving cell configuration.
  serving_cell_config ue_ded_serv_cell_cfg;

  /// Parameters to initialize/build the \c phy_cell_group.
  phy_cell_group_params pcg_params;

  /// Parameters for PUCCH-Config generation.
  pucch_builder_params pucch_cfg;
};

} // namespace srsran
