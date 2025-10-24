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

#include "srsran/adt/bounded_integer.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include <optional>
#include <variant>

namespace srsran {

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class pucch_nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

inline unsigned format1_cp_step_to_uint(pucch_nof_cyclic_shifts opt)
{
  return static_cast<unsigned>(opt);
}

/// Collects the parameters for PUCCH Format 0 that can be configured.
struct pucch_f0_params {
  bounded_integer<unsigned, 1, 2> nof_symbols{2};
  bool                            intraslot_freq_hopping{false};
};

/// Collects the parameters for PUCCH Format 1 that can be configured.
struct pucch_f1_params {
  /// Number of possible Initial Cyclic Shifts, equally spaced within the range {0,...,11}, as per \c PUCCH-format1, in
  /// \c PUCCH-Config, TS 38.331.
  pucch_nof_cyclic_shifts nof_cyc_shifts{pucch_nof_cyclic_shifts::no_cyclic_shift};
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
  std::optional<unsigned> max_payload_bits;
  max_pucch_code_rate     max_code_rate{max_pucch_code_rate::dot_25};
  /// For intraslot-freq-hopping, \c nof_symbols must be set to 2.
  bool intraslot_freq_hopping{false};
};

/// Collects the parameters for PUCCH Format 3 that can be configured.
struct pucch_f3_params {
  bounded_integer<unsigned, 4, 14> nof_symbols{4};
  unsigned                         max_nof_rbs{1};
  /// Maximum payload in bits that can be carried by PUCCH Format 3. When this field is set, \c max_nof_rbs is ignored
  /// and the maximum number of RBs is computed according to \ref get_pucch_format3_max_nof_prbs.
  std::optional<unsigned> max_payload_bits;
  max_pucch_code_rate     max_code_rate{max_pucch_code_rate::dot_25};
  bool                    intraslot_freq_hopping{false};
  bool                    additional_dmrs{false};
  bool                    pi2_bpsk{false};
};

/// Collects the parameters for PUCCH Format 4 that can be configured.
struct pucch_f4_params {
  bounded_integer<unsigned, 4, 14> nof_symbols{14};
  max_pucch_code_rate              max_code_rate{max_pucch_code_rate::dot_25};
  bool                             intraslot_freq_hopping{false};
  bool                             additional_dmrs{false};
  bool                             pi2_bpsk{false};
  /// Indicates whether OCCs (as per \c PUCCH-format4, in \c PUCCH-Config, TS 38.331) are supported.
  bool             occ_supported{false};
  pucch_f4_occ_len occ_length{pucch_f4_occ_len::n2};
};

/// \brief Parameters for PUCCH configuration.
/// Defines the parameters that are used for the PUCCH configuration builder. These parameters are used to define the
/// number of PUCCH resources, as well as the PUCCH format-specific parameters.
struct pucch_builder_params {
  /// UE specific parameters. Use to set the number of resources per UE for HARQ-ACK reporting (not including SR/CSI
  /// dedicated resources). NOTE: by default, each UE is assigned 1 SR and 1 CSI resource.
  /// \remark Format 0 and Format 1 resources are mutually exclusive.
  /// \remark Format 2 and Format 3 and Format 4 resources are mutually exclusive.
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f0_or_f1_res_harq       = 6;
  bounded_integer<unsigned, 1, 8> nof_ue_pucch_f2_or_f3_or_f4_res_harq = 6;
  /// \brief Number of separate PUCCH resource sets for HARQ-ACK reporting that are available in a cell.
  /// \remark UEs will be distributed possibly over different HARQ-ACK PUCCH sets; the more sets, the fewer UEs will
  /// have to share the same set, which reduces the chances that UEs won't be allocated PUCCH due to lack of resources.
  /// However, the usage of PUCCH-dedicated REs will be proportional to the number of sets.
  unsigned nof_cell_harq_pucch_res_sets = 1;
  /// Defines how many PUCCH F0 or F1 resources should be dedicated for SR at cell level; each UE will be allocated 1
  /// resource for SR.
  unsigned nof_sr_resources = 2;
  /// Defines how many PUCCH F2 or F3 or F4 resources should be dedicated for CSI at cell level; each UE will be
  /// allocated 1 resource for CSI.
  unsigned nof_csi_resources = 1;

  /// PUCCH Format specific parameters.
  // NOTE: Having \c pucch_f1_params first forces the variant to use the Format 1 in the default constructor.
  std::variant<pucch_f1_params, pucch_f0_params>                  f0_or_f1_params;
  std::variant<pucch_f2_params, pucch_f3_params, pucch_f4_params> f2_or_f3_or_f4_params;
  /// Maximum number of symbols per UL slot dedicated for PUCCH.
  /// \remark In case of Sounding Reference Signals (SRS) being used, the number of symbols should be reduced so that
  /// the PUCCH resources do not overlap in symbols with the SRS resources.
  /// \remark This parameter should be computed by the GNB and not exposed to the user configuration interface.
  bounded_integer<unsigned, 1, 14> max_nof_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
};

} // namespace srsran
