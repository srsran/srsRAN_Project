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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"

namespace asn1 {
namespace f1ap {

struct served_cell_info_s;

}
} // namespace asn1

namespace srsran {
namespace srs_du {

asn1::f1ap::served_cell_info_s make_asn1_served_cell_info(const du_served_cell_info& served_cell,
                                                          span<const s_nssai_t>      slices);

}
} // namespace srsran
