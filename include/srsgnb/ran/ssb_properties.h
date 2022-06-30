/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_SSB_PROPERTIES_H
#define SRSGNB_RAN_SSB_PROPERTIES_H

namespace srsgnb {

/// \brief SSB periodicity in milliseconds as per TS38.331 Section 6.3.2 IE ssb-periodicityServingCell.
enum class ssb_periodicity { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// \brief Converts the SSB periodicity property to its corresponding value in milliseconds.
inline unsigned ssb_periodicity_to_value(ssb_periodicity periodicity)
{
  return static_cast<unsigned>(periodicity);
}

/// \brief PSS power level allocation in dB, relative to SSS as per TS38.213 Section 4.1.
enum class ssb_beta_pss { dB_0, dB_3 };

/// \brief Describes the different SS/PBCH block patterns defined in 38.213 section 4.1 Cell search.
enum class ssb_pattern_case { A, B, C, D, E, invalid };

} // namespace srsgnb

#endif // SRSGNB_RAN_SSB_PROPERTIES_H
