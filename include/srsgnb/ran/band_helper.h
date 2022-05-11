/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_BAND_HELPER_H
#define SRSGNB_BAND_HELPER_H

/// This header is currently used only by the MAC to compute extra SSB parameters (needed for scheduling) from those
/// provided by DU.

#include <stdint.h>

namespace srsgnb {

enum class duplex_mode;
enum class subcarrier_spacing;
enum class ssb_pattern_case;

namespace band_helper {

/// \brief     Gets the NR band duplex mode.
/// \param[in] band Given band.
/// \return    A valid duplex_mode if the band is valid, duplex_mode::INVALID otherwise.
duplex_mode get_duplex_mode(uint16_t band);

/// \brief     Gets the lowest band that includes a given Downlink ARFCN.
/// \remark    Some bands can be subset of others, e.g., band 2 is a subset of band 25.
/// \param[in] arfcn Given Downlink ARFCN.
/// \return    The band number if the ARFCN is bounded in a band, UINT16_MAX otherwise.
uint16_t get_band_from_dl_arfcn(uint32_t arfcn);

/// \brief     Selects the SSB pattern case according to the band number and subcarrier spacing.
/// \remark    Described by TS 38.101-1 Table 5.4.3.3-1: Applicable SS raster entries per operating band.
/// \param[in] band NR Band number.
/// \param[in] scs SSB Subcarrier spacing.
/// \return    The SSB pattern case if band and subcarrier spacing match, invalid otherwise.
ssb_pattern_case get_ssb_pattern(uint16_t band, subcarrier_spacing scs);

/// \brief     Returns boolean indicating whether the band is in paired spectrum.
/// \remark    Paired spectrum is FDD, unpaired spectrum is TDD, SUL, SDL.
/// \param[in] band Given band.
/// \return    true for paired specrum, false otherwise.
bool is_paired_spectrum(uint16_t band);

} // namespace band_helper

} // namespace srsgnb

#endif // SRSGNB_BAND_HELPER_H