/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_PCI_H
#define SRSGNB_RAN_PCI_H

#include <cstdint>

namespace srsgnb {

/// 3GPP TS 38.331, PhysCellId ::= INTEGER (0..1007)
using pci_t         = uint16_t;
const pci_t MAX_PCI = 1007;

} // namespace srsgnb

#endif // SRSGNB_RAN_PCI_H
