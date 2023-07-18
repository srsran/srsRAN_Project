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

#include "srsran/adt/expected.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/nr_band.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

namespace band_helper {

error_type<std::string> is_gscn_valid_given_band(unsigned                 gscn,
                                                 nr_band                  band,
                                                 subcarrier_spacing       ssb_scs,
                                                 bs_channel_bandwidth_fr1 bw = bs_channel_bandwidth_fr1::invalid);

span<const unsigned> get_gscn_special_raster_iterator(nr_band            band,
                                                      subcarrier_spacing ssb_scs = subcarrier_spacing::invalid);

/// \brief Returns the \f$SS_{ref}\f$ from the SSB's GSCN, as per Section 5.4.3.1, TS 38.104.
/// \param[in] gscn GSCN value, as defined in TS 38.104, Section 5.4.3.1.
/// \return The the \f$SS_{ref}\f$ corresponding to the GSCN
double get_ss_ref_from_gscn(unsigned gscn);

optional<unsigned> get_gscn_from_ss_ref(double ss_ref_hz);

} // namespace band_helper
} // namespace srsran
