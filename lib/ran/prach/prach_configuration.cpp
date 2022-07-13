/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/prach/prach_configuration.h"

using namespace srsgnb;

static prach_configuration prach_configuration_get_fr1_paired(uint8_t prach_config_index)
{
  // TS38.211 Table 6.3.3.2-2.
  static const std::array<prach_configuration, 87> table = {{
      {preamble_format::FORMAT0, 16, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 16, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 16, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 16, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 8, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 8, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 8, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 8, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 4, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 4, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 4, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 4, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 2, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 2, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 2, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 2, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {1, 6}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {2, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {3, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {1, 4, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {2, 5, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {3, 6, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {0, 2, 4, 6, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {1, 3, 5, 7, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT0, 1, 0, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 16, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 16, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 16, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 16, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 8, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 8, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 8, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 8, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 4, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 4, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 4, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 4, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 2, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 2, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 2, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 2, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {1, 6}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {2, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {3, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {1, 4, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {2, 5, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT1, 1, 0, {3, 6, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 16, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 8, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 4, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 2, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 2, 0, {5}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 1, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT2, 1, 0, {5}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 16, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 16, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 16, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 16, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 8, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 8, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 8, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 4, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 4, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 4, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 4, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 2, 1, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 2, 1, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 2, 1, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 2, 1, {9}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {1}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {4}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {1, 6}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {2, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {3, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {1, 4, 7}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {2, 5, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {3, 6, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {0, 2, 4, 6, 8}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {1, 3, 5, 7, 9}, 0, 0, 0, 0},
      {preamble_format::FORMAT3, 1, 0, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, 0, 0, 0, 0},
  }};

  if (prach_config_index < table.size()) {
    return table[prach_config_index];
  }

  return PRACH_CONFIG_RESERVED;
}

prach_configuration srsgnb::prach_configuration_get(frequency_range fr, duplex_mode dm, uint8_t prach_config_index)
{
  if (fr == frequency_range::FR1 && (dm == duplex_mode::FDD || dm == duplex_mode::SUL)) {
    return prach_configuration_get_fr1_paired(prach_config_index);
  }

  return PRACH_CONFIG_RESERVED;
}