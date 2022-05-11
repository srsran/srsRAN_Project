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

#ifndef SRSGNB_RADIO_RADIO_CONSTANTS_H
#define SRSGNB_RADIO_RADIO_CONSTANTS_H

namespace srsgnb {

/// Maximum number of channels per stream in a radio baseband unit.
static constexpr unsigned RADIO_MAX_NOF_CHANNELS = 4;

/// Maximum number of streams that a radio baseband unit can support.
static constexpr unsigned RADIO_MAX_NOF_STREAMS = 8;

/// Total maximum number of ports that a radio baseband unit can support.
static constexpr unsigned RADIO_MAX_NOF_PORTS = RADIO_MAX_NOF_CHANNELS * RADIO_MAX_NOF_STREAMS;

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_CONSTANTS_H
