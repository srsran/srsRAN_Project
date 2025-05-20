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

#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"

namespace srsran {

/// Cell commanded by the CU to be activated via the F1AP interface.
struct f1ap_cell_to_activate {
  nr_cell_global_id_t  cgi;
  std::optional<pci_t> pci;
};

/// Cell failed to be activated via the F1AP interface.
struct f1ap_cell_failed_to_activate {
  nr_cell_global_id_t cgi;
  f1ap_cause_t        cause;
};

/// Cell commanded by the CU to be deactivated via the F1AP interface.
struct f1ap_cell_to_deactivate {
  nr_cell_global_id_t cgi;
};

} // namespace srsran
