/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/drx_config.h"
#include "srsran/adt/to_array.h"

using namespace srsran;

span<const std::chrono::milliseconds> drx_helper::valid_long_cycle_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr auto values =
      to_array<msec>({msec{10},   msec{20},   msec{32},   msec{40},   msec{60},   msec{64},   msec{70},
                      msec{80},   msec{128},  msec{160},  msec{256},  msec{320},  msec{512},  msec{640},
                      msec{1024}, msec{1280}, msec{2048}, msec{2560}, msec{5120}, msec{10240}});
  return values;
}

span<const std::chrono::milliseconds> drx_helper::valid_on_duration_timer_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr auto values =
      to_array<msec>({msec{1},   msec{2},   msec{3},   msec{4},   msec{5},   msec{6},    msec{8},    msec{10},
                      msec{20},  msec{30},  msec{40},  msec{50},  msec{60},  msec{80},   msec{100},  msec{200},
                      msec{300}, msec{400}, msec{500}, msec{600}, msec{800}, msec{1000}, msec{1200}, msec{1600}});
  return values;
}

span<const std::chrono::milliseconds> drx_helper::valid_inactivity_timer_values()
{
  using msec = std::chrono::milliseconds;
  static constexpr auto values =
      to_array<msec>({msec{0},   msec{1},   msec{2},   msec{3},   msec{4},    msec{5},    msec{6},   msec{8},
                      msec{10},  msec{20},  msec{30},  msec{40},  msec{50},   msec{60},   msec{80},  msec{100},
                      msec{200}, msec{300}, msec{500}, msec{750}, msec{1280}, msec{1920}, msec{2560}});
  return values;
}

span<const unsigned> drx_helper::valid_retx_timer_values()
{
  static constexpr auto values = to_array<unsigned>({0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320});
  return values;
}
