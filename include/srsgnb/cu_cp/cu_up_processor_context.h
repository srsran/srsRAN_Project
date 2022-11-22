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

#include "srsgnb/cu_cp/cu_cp_types.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

struct cu_up_processor_context {
  cu_up_index_t cu_up_index;   /// Index assisgned by CU-CP
  uint64_t      id;            /// the gNB-CU-UP-ID
  std::string   name = "none"; /// gNB-CU-UP-Name
};

} // namespace srs_cu_cp
} // namespace srsgnb
