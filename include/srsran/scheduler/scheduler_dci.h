/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "vrb_alloc.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"
#include <cstdint>

namespace srsran {

struct cce_position {
  /// CCE start index. Values: (0..135).
  unsigned ncce;
  /// Aggregation level of the DCI.
  aggregation_level aggr_lvl;
};

enum class dci_dl_format { f1_0, f1_1, f2_0 };
enum class dci_ul_format { f0_0, f0_1 };

/// Defines which fields are stored in the DCI payload, based on the chosen DCI format and RNTI type.
enum class dci_dl_rnti_config_type { si_f1_0, ra_f1_0, c_rnti_f1_0, tc_rnti_f1_0, p_rnti_f1_0, c_rnti_f1_1 };

inline const char* dci_dl_rnti_config_rnti_type(dci_dl_rnti_config_type type)
{
  std::array<const char*, 6> rnti_types = {"si-rnti", "ra-rnti", "c-rnti", "tc-rnti", "p-rnti", "c-rnti"};
  return (unsigned)type < rnti_types.size() ? rnti_types[(unsigned)type] : "invalid";
}

inline const char* dci_dl_rnti_config_format(dci_dl_rnti_config_type type)
{
  if (type != dci_dl_rnti_config_type::c_rnti_f1_1) {
    return "1_0";
  }
  return "1_1";
}

/// \brief Describes an unpacked DL DCI message.
/// \remark See FAPI DCI PDU and ORAN WG8 DL-DCI Configuration.
struct dci_dl_info {
  dci_dl_rnti_config_type type;
  union {
    dci_1_0_si_rnti_configuration si_f1_0;
    dci_1_0_ra_rnti_configuration ra_f1_0;
    dci_1_0_c_rnti_configuration  c_rnti_f1_0;
    dci_1_0_tc_rnti_configuration tc_rnti_f1_0;
    dci_1_0_p_rnti_configuration  p_rnti_f1_0;
    dci_1_1_configuration         c_rnti_f1_1;
  };

  dci_dl_info() : type(dci_dl_rnti_config_type::si_f1_0) { new (&si_f1_0) dci_1_0_si_rnti_configuration(); }
};

/// Defines which fields are stored in the DCI payload, based on the chosen DCI format and RNTI type.
enum class dci_ul_rnti_config_type { tc_rnti_f0_0, c_rnti_f0_0, c_rnti_f0_1 };

inline const char* dci_ul_rnti_config_rnti_type(dci_ul_rnti_config_type type)
{
  std::array<const char*, 3> rnti_types = {"tc-rnti", "c-rnti", "c-rnti"};
  return (unsigned)type < rnti_types.size() ? rnti_types[(unsigned)type] : "invalid";
}

inline const char* dci_ul_rnti_config_format(dci_ul_rnti_config_type type)
{
  if (type != dci_ul_rnti_config_type::c_rnti_f0_1) {
    return "0_0";
  }
  return "0_1";
}

struct dci_ul_info {
  dci_ul_rnti_config_type type;
  union {
    dci_0_0_c_rnti_configuration  c_rnti_f0_0;
    dci_0_0_tc_rnti_configuration tc_rnti_f0_0;
    dci_0_1_configuration         c_rnti_f0_1;
  };

  dci_ul_info() : type(dci_ul_rnti_config_type::c_rnti_f0_0) { new (&c_rnti_f0_0) dci_0_0_c_rnti_configuration(); }
};

} // namespace srsran
