/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"

namespace srsran {

std::string         e2sm_kpm_label_2_str(e2sm_kpm_label_enum label);
std::string         e2sm_kpm_scope_2_str(e2sm_kpm_metric_level_enum level);
e2sm_kpm_label_enum asn1_label_2_enum(const asn1::e2sm_kpm::meas_label_s& meas_label);

} // namespace srsran
