
#ifndef SRSGNB_MAC_CELL_ASN1_HELPERS_H
#define SRSGNB_MAC_CELL_ASN1_HELPERS_H

#include "srsgnb/mac/cell_configuration.h"

// ASN.1 forward declarations
namespace asn1 {
namespace rrc_nr {

struct serving_cell_cfg_common_sib_s;
struct mib_s;
}
} // namespace asn1

namespace srsgnb {

/// Fills mac cell configuration with parameters in ASN.1 ServingCellConfigCommonSIB
void fill_mac_cell_cfg_from_asn1(const asn1::rrc_nr::serving_cell_cfg_common_sib_s& in,
                                 const asn1::rrc_nr::mib_s&                         mib_in,
                                 mac_cell_configuration&                            out);

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_ASN1_HELPERS_H
