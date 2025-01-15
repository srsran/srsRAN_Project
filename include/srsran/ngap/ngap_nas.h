/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

struct ngap_dl_nas_transport_message {
  ue_index_t  ue_index = ue_index_t::invalid;
  byte_buffer nas_pdu;
  bool        ue_cap_info_request = false;
};

} // namespace srs_cu_cp
} // namespace srsran
