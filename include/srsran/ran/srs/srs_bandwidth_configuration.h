/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>
#include <optional>

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
/// \return A valid SRS configuration if the provided arguments are valid, otherwise \c std::nullopt.
std::optional<srs_configuration> srs_configuration_get(uint8_t c_srs, uint8_t b_srs);

} // namespace srsran
