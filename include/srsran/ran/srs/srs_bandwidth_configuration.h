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

#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Contains the Sounding Reference Signals (SRS) bandwidth configuration parameters.
///
/// The parameters are used in TS38.211 Section 6.4.1.4.3 and they are derived from TS38.211 Table 6.4.1.4.3-1.
struct srs_configuration {
  /// Sounding Reference Signal bandwidth in physical resource blocks, corresponds to the parameter \f$m_{SRS,b}\f$.
  unsigned m_srs;
  /// Number of frequency hopping locations, corresponds to parameter \f$N_{b}\f$.
  unsigned N;
};

/// \brief Gets an SRS configuration.
///
/// \param[in] c_srs Parameter \f$C_{SRS}\f$ contained in the higher-layer parameter \e freqHopping (see TS38.331
/// Section 6.3.2, Information Element \e SRS-Config).
/// \param[in] b_srs Parameter \f$B_{SRS}\f$ contained in the higher-layer parameter \e freqHopping (see TS38.331
/// Section 6.3.2, Information Element \e SRS-Config).
/// \return A valid SRS configuration if the provided arguments are valid, otherwise \c nullopt.
optional<srs_configuration> srs_configuration_get(uint8_t c_srs, uint8_t b_srs);

} // namespace srsran
