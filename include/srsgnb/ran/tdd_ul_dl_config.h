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

#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// \remark See TS 38.331, "TDD-UL-DL-Pattern".
struct tdd_ul_dl_pattern {
  /// Periodicity of the DL-UL pattern.
  unsigned dl_ul_tx_period_nof_slots;
  /// Values: (0..maxNrofSlots=320).
  unsigned nof_dl_slots;
  /// Values: (0..maxNrofSymbols-1=13).
  unsigned nof_dl_symbols;
  /// Values: (0..maxNrofSlots=320).
  unsigned nof_ul_slots;
  /// Values: (0..maxNrofSymbols-1=13).
  unsigned nof_ul_symbols;
};

/// \remark See TS 38.331, "TDD-UL-DL-ConfigCommon".
struct tdd_ul_dl_config_common {
  /// Reference SCS used to determine the time domain boundaries in the UL-DL pattern. The network ensures that
  /// this value is not larger than any SCS of configured BWPs for the serving cell.
  subcarrier_spacing          ref_scs;
  tdd_ul_dl_pattern           pattern1;
  optional<tdd_ul_dl_pattern> pattern2;
};

} // namespace srsgnb
