
#ifndef SRSGNB_RAN_PCI_H
#define SRSGNB_RAN_PCI_H

#include <cstdint>

namespace srsgnb {

/// 3GPP TS 38.331, PhysCellId ::= INTEGER (0..1007)
using pci_t         = uint16_t;
const pci_t MAX_PCI = 1007;

} // namespace srsgnb

#endif // SRSGNB_RAN_PCI_H
