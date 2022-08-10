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

#include "../../lib/du_manager/converters/config_generator.h"

namespace srsgnb {

void test_ssb_coreset0_allocation(unsigned                            dl_arfcn,
                                  nr_band                             nr_band,
                                  unsigned                            n_rbs_,
                                  subcarrier_spacing                  scs_common,
                                  subcarrier_spacing                  scs_ssb,
                                  const srs_du::du_ssb_sib1_location& params);

} // namespace srsgnb
