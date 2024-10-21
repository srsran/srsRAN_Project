/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

/// DL/UL UE Associated NRPPA Transport message.
struct ngap_ue_associated_nrppa_transport {
  ue_index_t  ue_index;
  byte_buffer routing_id;
  byte_buffer nrppa_pdu;
};

/// DL/UL Non UE Associated NRPPA Transport message.
struct ngap_non_ue_associated_nrppa_transport {
  byte_buffer routing_id;
  byte_buffer nrppa_pdu;
};

} // namespace srs_cu_cp
} // namespace srsran
