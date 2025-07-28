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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/bounded_integer.h"

namespace srsran {

/// \brief CSI Rank Indicator restriction type.
///
/// The RI restriction field is described in TS38.331 Section 6.3.2, Information Element \e CodebookConfig. The
/// bit-width of this field must be equal to the number of CSI-RS transmit number of antennas.
using ri_restriction_type = bounded_bitset<8>;

/// CSI-RS PMI report codebook configuration.
enum class pmi_codebook_type {
  /// One CSI-RS antenna port.
  one = 0,
  /// Two CSI-RS antenna ports.
  two,
  /// PMI codebook TypeI, single-panel with four CSI-RS antenna ports and mode 1.
  typeI_single_panel_4ports_mode1,
  /// Any other configuration.
  other
};

/// \brief CSI report quantities.
///
/// Described in TS38.331 Section 6.3.2, Information Element \e CSI-ReportConfig.
enum class csi_report_quantities {
  /// CSI report contains the quantities CRI, RI, PMI and wideband CQI.
  cri_ri_pmi_cqi = 0,
  /// CSI report contains the quantities CRI, RI and wideband CQI.
  cri_ri_cqi,
  /// CSI report contains the quantities CRI, RI, LI, PMI and wideband CQI.
  cri_ri_li_pmi_cqi,
  /// Other CSI report quantity configuration. Not supported.
  other
};

/// Collects Channel State Information (CSI) report subband configuration parameters.
struct csi_report_subband_configuration {
  /// Enable subband CQI field.
  bool cqi;
  /// Enabled subband PMI field.
  bool pmi;
  /// \brief Number of subbands in {5..18}.
  ///
  /// Range deduced from TS38.214 Table 5.2.1.4-2. The minimum number of subbands is 5 and the maximum 18.
  bounded_integer<uint8_t, 5, 18> nof_subbands;
};

/// Collects Channel State Information (CSI) report configuration parameters.
struct csi_report_configuration {
  /// \brief Number of CSI-RS resources in the corresponding resource set.
  ///
  /// Corresponds to parameter \f$K^\testup{CSI-RS}_\testup{s}\f$ as per TS38.212 Section 6.3.1.1.2.
  unsigned nof_csi_rs_resources;
  /// CSI-RS PMI codebook configuration.
  pmi_codebook_type pmi_codebook;
  /// Set to true the bits in position \f$i\f$ to enable RI reporting for \f$\upsilon + i\f$.
  ri_restriction_type ri_restriction;
  /// Select CSI report quantities.
  csi_report_quantities quantities;
  /// Subband configuration parameters. Set to \c std::nullopt for no subband reporting.
  std::optional<csi_report_subband_configuration> subband;
};

inline const char* to_string(pmi_codebook_type codebook)
{
  switch (codebook) {
    case pmi_codebook_type::one:
      return "one-port";
    case pmi_codebook_type::two:
      return "two-port";
    case pmi_codebook_type::typeI_single_panel_4ports_mode1:
      return "TypeI-SinglePanel-FourPorts-Mode1";
    case pmi_codebook_type::other:
    default:
      return "other";
  }
}

inline const char* to_string(csi_report_quantities quantities)
{
  switch (quantities) {
    case srsran::csi_report_quantities::cri_ri_pmi_cqi:
      return "cri-ri-pmi-cqi";
    case srsran::csi_report_quantities::cri_ri_cqi:
      return "cri-ri-cqi";
    case srsran::csi_report_quantities::cri_ri_li_pmi_cqi:
      return "cri-ri-li-pmi-cqi";
    case srsran::csi_report_quantities::other:
    default:
      return "other";
  }
}

} // namespace srsran
