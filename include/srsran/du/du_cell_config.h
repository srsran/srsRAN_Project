/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/ran/tdd_ul_dl_config.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {

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
  bounded_integer<unsigned, 1, 2> nof_symbols{1};
  unsigned                        max_nof_rbs{1};
  /// Maximum payload in bits that can be carried by PUCCH Format 2. When this field is set, \c max_nof_rbs is ignored
  /// and the maximum number of RBs is computed according to \ref get_pucch_format2_max_nof_prbs.
  optional<unsigned>  max_payload_bits;
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};
  /// For intraslot-freq-hopping, \c nof_symbols must be set to 2.
  bool intraslot_freq_hopping{false};
};

/// Defines the parameters that are used for the PUCCH configuration builder. These parameters are used to define the
/// PUCCH resources per cell and per UE, as well as the PUCCH format-specific parameters.
struct pucch_builder_params {
  struct nof_pucch_resources {
    unsigned nof_f1;
    unsigned nof_f2;
  };
  struct max_nof_pucch_rbs {
    unsigned max_f1_rbs;
    unsigned max_f2_rbs;
  };

  /// Cell parameters.
  /// Defines the total number of resources (including SR and CSI) that will be generated as part of the cell resource
  /// list. If \ref max_pucch_rbs is set, nof_cell_resources is ignored.
  nof_pucch_resources nof_cell_resources;
  /// Defines the total number of RBs that will be used for PUCCH resources (including SR and CSI). If this is set, then
  /// the \ref nof_cell_resources is ignored.
  optional<max_nof_pucch_rbs> max_pucch_rbs;
  /// Defines how many PUCCH F1 resources should be dedicated for SR at cell level.
  unsigned nof_sr_resources;

  /// UE specific parameters. Use to set the number of resources per UE for HARQ-ACK reporting (not including SR/CSI
  /// dedicated resources). NOTE: by default, each UE is assigned 1 SR and 1 CSI resource.
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f1_res_harq;
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f2_res_harq;

  /// PUCCH Format specific parameters.
  pucch_f1_params f1_params;
  pucch_f1_params f2_params;
};

/// Cell Configuration, including common and UE-dedicated configs, that the DU will use to generate other configs for
/// other layers (e.g. scheduler).
struct du_cell_config {
  pci_t               pci;
  uint32_t            tac;
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

  /// Cell-specific DL and UL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;

  /// UE-dedicated serving cell configuration.
  serving_cell_config ue_ded_serv_cell_cfg;

  /// Parameters for PUCCH-Config generation.
  pucch_builder_params pucch_cfg;
};

} // namespace srsran
