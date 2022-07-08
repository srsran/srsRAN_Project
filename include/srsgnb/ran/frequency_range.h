/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_FREQUENCY_RANGE_H
#define SRSGNB_RAN_FREQUENCY_RANGE_H

namespace srsgnb {

/// Labels for the frequency ranges described in TS38.104 Table 5.1-1.
enum class frequency_range {
  /// Frequency range 1, from 410 to 7125 MHz.
  FR1,
  /// Frequency range 2, from 24250 to 52600 MHz.
  FR2
};

} // namespace srsgnb

#endif // SRSGNB_RAN_FREQUENCY_RANGE_H
