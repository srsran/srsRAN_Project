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

#include "srsran/adt/expected.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

namespace band_helper {

/// \brief Validate the SSB GSCN for a given band, as per Section 5.4.3.1, TS 38.104.
/// \param[in] gscn GSCN value, as defined in TS 38.104, Section 5.4.3.1.
/// \param[in] band NR band.
/// \param[in] ssb_scs SSB subcarrier spacing.
/// \param[in] bw Channel Bandwidth in MHz, which is required to validate some bands' GSCN values.
/// \return If the GSCN is invalid for the band, a std::string value is returned with the reason.
error_type<std::string> is_gscn_valid_given_band(unsigned                 gscn,
                                                 nr_band                  band,
                                                 subcarrier_spacing       ssb_scs,
                                                 bs_channel_bandwidth_fr1 bw = bs_channel_bandwidth_fr1::invalid);

/// \brief Get an iterator to the GSCN values for bands with special GSCN rasters.
///
/// This is used only for bands n34, n38, n39 (with SCS 15kHz) and n46, n96, n102.
/// \param[in] band NR band.
/// \param[in] ssb_scs SSB subcarrier spacing.
/// \param[in] bw Channel Bandwidth in MHz, which is required to validate some bands' GSCN values.
span<const unsigned> get_gscn_special_raster_iterator(nr_band            band,
                                                      subcarrier_spacing ssb_scs = subcarrier_spacing::invalid);

/// \brief Returns the \f$SS_{ref}\f$ from the SSB's GSCN, as per Section 5.4.3.1, TS 38.104.
/// \param[in] gscn GSCN value, as defined in TS 38.104, Section 5.4.3.1.
/// \return The \f$SS_{ref}\f$ corresponding to the GSCN.
double get_ss_ref_from_gscn(unsigned gscn);

/// \brief Returns GSCN from the \f$SS_{ref}\f$, as per Section 5.4.3.1, TS 38.104.
/// \param[in] ss_ref_hz \f$SS_{ref}\f$ value in Hz, as defined in TS 38.104, Section 5.4.3.1.
/// \return The GSCN, if the \f$SS_{ref}\f$ is a valid value. Else, it returns an empty object.
optional<unsigned> get_gscn_from_ss_ref(double ss_ref_hz);

} // namespace band_helper

} // namespace srsran
