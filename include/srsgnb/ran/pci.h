
#ifndef SRSGNB_PCI_H
#define SRSGNB_PCI_H

#include <cstdint>

namespace srsgnb {

/// Physical cell ID. [3GPP TS 38.211 [2], sec 7.4.2.1], N_{ID}^cell. Value: 0->1007
using pci_t             = uint16_t;
const pci_t INVALID_PCI = 1008;

} // namespace srsgnb

#endif // SRSGNB_PCI_H
