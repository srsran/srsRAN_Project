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

#include "srsran/ran/nr_cgi.h"

namespace srsran {

/// Cell commanded by the CU to be activated via the F1AP interface.
struct f1ap_cell_to_activate {
  nr_cell_global_id_t cgi;
};

} // namespace srsran
