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

#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran {

struct prach_configuration;

/// \brief Collects PRACH preamble information parameters.
///
/// The parameters are used and described in TS38.211 Section 6.3.3.1.
struct prach_preamble_information {
  /// Sequence length in frequency domain, parameter \f$L_{RA}\f$.
  unsigned sequence_length;
  /// Parameter \f$\Delta f^{RA}\f$.
  prach_subcarrier_spacing scs;
  /// Parameter \f$N_u\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit symbol_length;
  /// Parameter \f$N_{CP}^{RA}\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit cp_length;
  /// Flag: true if the preamble supports the restricted sets A and B.
  bool support_restricted_sets;
};

/// \brief Get long PRACH preamble information as per TS38.211 Table 6.3.3.1-1.
///
/// An assertion is triggered if the PRACH preamble format is short.
///
/// \param[in] format PRACH preamble format.
/// \return PRACH preamble information.
prach_preamble_information get_prach_preamble_long_info(prach_format_type format);

/// \brief Get short PRACH preamble information as per TS38.211 Table 6.3.3.1-2.
///
/// An assertion is triggered if:
/// - the PRACH preamble format is long, or
/// - the random access subcarrier spacing does not satisfy \f$\Delta f ^\textup{RA} = 15 \cdot 2^\mu\f$ where \f$\mu
/// \in \{0, 1, 2, 3\}\f$.
///
/// \param[in] format        PRACH preamble format.
/// \param[in] ra_scs        Random access subcarrier spacing \f$\Delta f ^\textup{RA}\f$.
/// \param[in] last_occasion Set to true if \f$n_\textup{t}^\textup{RA}=N_\textup{t}^\textup{RA,slot}-1\f$
/// \return PRACH preamble information.
prach_preamble_information
get_prach_preamble_short_info(prach_format_type format, prach_subcarrier_spacing ra_scs, bool last_occasion);

/// \brief Gets the PRACH window duration.
/// \param[in] format             PRACH preamble format.
/// \param[in] pusch_scs          Uplink resource grid subcarrier spacing. Determines the numerology \f$\mu\f$.
/// \param[in] start_symbol_index OFDM symbol index within the slot that marks the start of the acquisition window for
///                               the first time-domain PRACH occasion.
/// \param[in] nof_td_occasions   Number of time-domain occasions within a PRACH window.
/// \return The PRACH window duration from the beginning of the slot to the end of the last time-domain occasion.
/// \note The RA subcarrier spacing for short preambles is assumed to be equal to the uplink resource grid subcarrier
///       spacing.
phy_time_unit get_prach_window_duration(prach_format_type  format,
                                        subcarrier_spacing pusch_scs,
                                        unsigned           start_symbol_index,
                                        unsigned           nof_td_occasions);

/// \brief Collects PRACH preamble duration information.
struct prach_symbols_slots_duration {
  /// Duration of the PRACH Preamble in slots, with reference to the PUSCH SCS.
  unsigned prach_length_slots;
  /// PRACH starting slot within the subframe, with reference to the PUSCH SCS. For 15kHz PUSCH SCS, values: {0}; for
  /// 30kHz PUSCH SCS, values: {0, 1}
  unsigned start_slot_pusch_scs;
  /// PRACH duration in symbols, with reference to the PUSCH SCS.
  unsigned nof_symbols;
  /// PRACH starting symbol within the slot, with reference to the PUSCH SCS.
  unsigned start_symbol_pusch_scs;
};

/// \brief Compute PRACH preamble duration information.
///
/// \param[in] prach_cfg PRACH preamble configuration.
/// \param[in] pusch_scs PUSCH SCS that is used as a reference for symbol and slot unit.
/// \return PRACH preamble duration information.
prach_symbols_slots_duration get_prach_duration_info(const prach_configuration& prach_cfg,
                                                     subcarrier_spacing         pusch_scs);

} // namespace srsran
