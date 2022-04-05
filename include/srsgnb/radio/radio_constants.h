

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
