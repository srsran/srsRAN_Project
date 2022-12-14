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

namespace srsgnb {

/// \brief Maps the (PUSCH) SNR to a given MCS for PUSCH.
///
/// The objective of this function is to find the maximum MCS that can be used for a given SNR.
/// TODO: revise this function once the SNR to BLER curves will have been prepared.
sch_mcs_index map_snr_to_mcs_ul(double snr);

} // namespace srsgnb
