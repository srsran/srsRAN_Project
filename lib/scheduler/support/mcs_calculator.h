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

#include "srsgnb/ran/sch_mcs.h"
#include "tbs_calculator.h"

namespace srsgnb {

struct ul_mcs {
  sch_mcs_index mcs;
  unsigned      tbs;
};

unsigned map_snr_to_mcs(double snr);

ul_mcs get_ul_mcs(tbs_calculator_configuration tbs_params, double ul_snr);

} // namespace srsgnb
