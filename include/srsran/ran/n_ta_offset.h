/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
