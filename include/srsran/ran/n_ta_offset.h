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

namespace srsran {

/// \brief Time advance offset - parameter \f$N_{TA,offset}\f$ in TS38.211 Section 4.3.3.
///
/// This value must be selected from the parameter \e n-TimingAdvanceOffset (TS38.331 Section 6.3.2, Information Element
/// \e ServingCellConfigCommon) if it is present. Otherwise, it is given by TS38.133 Section 7.1.2 depending on the
/// duplex mode of the cell and the frequency range.
///
/// The values are given in units of \f$T_c\f$ (see TS38.211 Section 4.1).
enum class n_ta_offset {
  /// For FR1 FDD band with LTE-NR coexistence case.
  n0 = 0,
  /// For FR1 FDD band without LTE-NR coexistence case or for FR1 TDD band without LTE-NR coexistence case.
  n25600 = 25600,
  /// For FR2.
  n13792 = 13792,
  /// For FR1 TDD band with LTE-NR coexistence case.
  n39936 = 39936
};

} // namespace srsran
