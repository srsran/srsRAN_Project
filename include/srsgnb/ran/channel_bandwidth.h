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

/// Labels for the BS Channel BW for FR, described in TS38.104 Table Table 5.3.2-1.
enum class bs_channel_bw_fr1 {
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

/// Converts SCS into integer in kHz.
constexpr inline unsigned bs_ch_bw_to_uint(bs_channel_bw_fr1 bw)
{
  return static_cast<unsigned>(bw);
}

} // namespace srsgnb