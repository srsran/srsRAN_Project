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

#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/modulation_scheme.h"
#include "srsran/ran/sch_mcs.h"

namespace srsran {

/// Precoding Matrix Indicator (PMI).
struct csi_report_pmi {
  /// PMI field for two antenna port.
  struct two_antenna_port {
    /// PMI codebook index from TS38.214 Table 5.2.2.2.1-1
    unsigned pmi;
  };

  /// \brief PMI field for Type I Single-Panel codebook, mode 1, and four antenna ports.
  ///
  /// This PMI codebook mode is described in TS38.214 Section 5.2.2.2.1.
  struct typeI_single_panel_4ports_mode1 {
    /// PMI parameter \f$i_{1,1}\f$.
    unsigned i_1_1;
    /// PMI parameter \f$i_{1,2}\f$.
    unsigned i_1_2;
    /// PMI parameter \f$i_{1,3}\f$. Only available for \f$\upsilon \in \{2,3,4\}\f$.
    optional<unsigned> i_1_3;
    /// PMI parameter \f$i_2\f$.
    unsigned i_2;
  };

  /// Actual PMI value.
  variant<two_antenna_port, typeI_single_panel_4ports_mode1> type;
};

/// Collects Chanel State Information (CSI) report fields.
struct csi_report_data {
  /// Rank Indicator (RI) data type.
  using ri_type = bounded_integer<uint8_t, 1, 8>;
  /// Layer Indicator (LI) data type.
  using li_type = bounded_integer<uint8_t, 0, 7>;
  /// Wideband Channel Quality Indicator (CQI) data type.
  using wideband_cqi_type = bounded_integer<uint8_t, 0, 15>;

  /// CSI-RS Resource Indicator (CRI) if reported.
  optional<uint8_t> cri;
  /// Rank Indicator (RI) if reported. The range is {1, ..., 8}.
  optional<ri_type> ri;
  /// Layer Indicator (LI) if reported.
  optional<li_type> li;
  /// PMI wideband information fields if reported.
  optional<csi_report_pmi> pmi;
  /// Wideband CQI for the first TB.
  optional<wideband_cqi_type> first_tb_wideband_cqi;
  /// Wideband CQI for the second TB.
  optional<wideband_cqi_type> second_tb_wideband_cqi;
};

} // namespace srsran
