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

#include "srsgnb/cu_cp/cu_cp_types.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

struct cu_up_processor_context {
  cu_up_index_t cu_up_index = cu_up_index_t::invalid; /// Index assisgned by CU-CP
  uint64_t      id;                                   /// the gNB-CU-UP-ID
  std::string   cu_up_name = "none";                  /// gNB-CU-UP-Name
  std::string   cu_cp_name = "none";                  /// gNB-CU-CP-Name
};

} // namespace srs_cu_cp
} // namespace srsran
