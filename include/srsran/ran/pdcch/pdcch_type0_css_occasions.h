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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Describes a Type0-PDCCH CSS monitoring occasion configuration parameters for pattern 1.
struct pdcch_type0_css_occasion_pattern1_configuration {
  /// Set to true for FR2, otherwise it uses FR1.
  bool is_fr2;
  /// Higher layer parameter \c searchSpaceZero as per TS38.331 PDCCH-ConfigSIB1.
  uint8_t ss_zero_index;
  /// Number of symbols of the CORESET, \f$N_{symb}^{CORESET}\f$.
  unsigned nof_symb_coreset;
};

/// \brief Describes a Type0-PDCCH CSS monitoring occasions for pattern 1.
///
/// The Type0-PDCCH CSS monitoring occasions are described in TS38.213 Section 13.
///
/// A UE monitors PDCCH in the Type0-PDCCH CSS set over two consecutive slots starting from slot \f$n_0\f$.
///
/// For SS/PBCH block with index \f$i\f$, the UE determines an index of slot \f$n_0=\left (O\cdot 2^\mu + \left \lfloor
/// i\cdot M \right \rfloor \right )mod N_{slot}^{frame,\mu}\f$ located in a system frame number \f$SFN_C\f$ satisfying:
///   - \f$SFN_C mod 2 = 0\f$ if \f$\left \lfloor \left ( O \cdot 2^\mu + \left \lfloor i\cdot M \right \rfloor \right )
///   / N_{slot}^{frame,\mu}\right \rfloor mod 2 = 0\f$, or
///   - \f$SFN_C mod 2 = 1\f$ if \f$\left \lfloor \left ( O \cdot 2^\mu + \left \lfloor i\cdot M \right \rfloor \right )
///   / N_{slot}^{frame,\mu}\right \rfloor mod 2 = 1\f$.
///
struct pdcch_type0_css_occasion_pattern1_description {
  /// Maximum number of Type0-PDCCH CSS monitoring occasions in a single slot.
  static constexpr unsigned MAX_NOF_OCCASIONS_PER_SLOT = 2;
  /// Parameter \f$O\f$.
  double offset;
  /// Parameter \f$M\f$.
  double M;
  /// \brief Indicates the starting symbol of the CORESET.
  ///
  /// If more than one occasion is available in a single slot, the first element corresponds to even \f$i\f$ and the
  /// second to odd \f$i\f$.
  static_vector<unsigned, MAX_NOF_OCCASIONS_PER_SLOT> start_symbol;
};

/// \brief Get Type0-PDCCH CSS occasions for multiplexing pattern 1 as per TS38.213 Section 13.
///
/// The Type0-PDCCH CSS occasions are retrieved from TS38.213 Table 13-11 for FR1 and Table 13-12 for FR2.
///
/// An assertion is triggered if \c config is not valid.
///
/// \param[in] config Provides the necessary parameters to calculate the occasions.
/// \return A valid Type0-PDCCH CSS occasion for multiplexing pattern 1.
pdcch_type0_css_occasion_pattern1_description
pdcch_type0_css_occasions_get_pattern1(const pdcch_type0_css_occasion_pattern1_configuration& config);

/// \brief Helper function that returns whether slot n0 of Type0-PDCCH CSS is in an even/odd frame.
/// \param[in] table_13_11_and_13_12_O Parameter \c O provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] table_13_11_and_13_12_M Parameter \c M provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] numerology_mu Numerlogy based on SCS for PDCCH receptions in the CORESET.
/// \param[in] ssb_index SS/PBCH block with index.
/// \return 0 if slot n0 is located in an even frame or 1 if slot n0 is located in an odd frame.
unsigned type0_pdcch_css_n0_is_even_frame(double   table_13_11_and_13_12_O,
                                          double   table_13_11_and_13_12_M,
                                          uint8_t  numerology_mu,
                                          unsigned ssb_index);

/// \brief Helper function that returns slot n0 (where UE should monitor Type0-PDCCH CSS) for a given SSB (beam) index.
/// \param[in] table_13_11_and_13_12_O Parameter \c O provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] table_13_11_and_13_12_M Parameter \c M provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] scs_common SCS for PDCCH receptions in the CORESET.
/// \param[in] ssb_index SS/PBCH block with index.
/// \return Returns slot n0 where UE should monitor Type0-PDCCH CSS.
slot_point get_type0_pdcch_css_n0(double             table_13_11_and_13_12_O,
                                  double             table_13_11_and_13_12_M,
                                  subcarrier_spacing scs_common,
                                  unsigned           ssb_index);

} // namespace srsran
