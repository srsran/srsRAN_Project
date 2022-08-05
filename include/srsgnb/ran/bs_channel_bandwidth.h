/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

/// Labels for the BS Channel Bandwidth for FR1, described in TS38.104, Table 5.3.2-1.
enum class bs_channel_bandwidth_fr1 {
  MHz5   = 5,
  MHz10  = 10,
  MHz15  = 15,
  MHz20  = 20,
  MHz25  = 25,
  MHz30  = 30,
  MHz40  = 40,
  MHz50  = 50,
  MHz60  = 60,
  MHz70  = 70,
  MHz80  = 80,
  MHz90  = 90,
  MHz100 = 100,
};

/// Converts the BS channel bandwidth label into the actual BW value in MHz.
constexpr inline unsigned bs_channel_bandwidth_to_MHz(bs_channel_bandwidth_fr1 bw)
{
  return static_cast<unsigned>(bw);
}

/// Labels for the <em>Minimum Channel Bandwidth for FR1<\em>. This is not explicitly defined in the TS, but it is used
/// by TS 38.213, Section 13 for the Coreset 0 configuration. As per TS 38.104, Table 5.3.5-1, there are three possible
/// minimum BW: 5MHz, 10MHz, and 40MHz.
enum class min_channel_bandwidth { MHz5 = 0, MHz10 = 1, MHz40 = 2, invalid = 3 };

} // namespace srsgnb