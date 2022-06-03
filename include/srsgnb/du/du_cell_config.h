/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CELL_CONFIG_H
#define SRSGNB_DU_CELL_CONFIG_H

#include "srsgnb/ran/pci.h"

namespace srsgnb {

/// Initial, common Cell Configuration used in DU.
struct du_cell_config {
  pci_t pci;
};

} // namespace srsgnb

#endif // SRSGNB_DU_CELL_CONFIG_H
