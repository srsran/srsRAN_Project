/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"

namespace srsran {

std::string         e2sm_kpm_label_2_str(e2sm_kpm_label_enum label);
std::string         e2sm_kpm_scope_2_str(e2sm_kpm_metric_level_enum level);
e2sm_kpm_label_enum asn1_label_2_enum(const asn1::e2sm_kpm::meas_label_s& meas_label);

// comparison operators
bool operator==(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs);
bool operator!=(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs);
bool operator<(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs);
bool operator==(asn1::e2sm_kpm::ueid_gnb_du_s const& lhs, asn1::e2sm_kpm::ueid_gnb_du_s const& rhs);
bool operator<(asn1::e2sm_kpm::ueid_gnb_du_s const& lhs, asn1::e2sm_kpm::ueid_gnb_du_s const& rhs);

} // namespace srsran
