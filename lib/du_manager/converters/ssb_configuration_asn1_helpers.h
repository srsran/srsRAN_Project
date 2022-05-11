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

#ifndef SRSGNB_SSB_CONFIGURATION_ASN1_HELPERS_H
#define SRSGNB_SSB_CONFIGURATION_ASN1_HELPERS_H

#include "srsgnb/ran/ssb_configuration.h"

// ASN.1 forward declarations
namespace asn1 {
namespace rrc_nr {

struct serving_cell_cfg_common_sib_s;
struct mib_s;

} // namespace rrc_nr
} // namespace asn1

namespace srsgnb {

void fill_ssb_configuration_from_asn1(ssb_configuration&                                 out,
                                      const asn1::rrc_nr::mib_s&                         mib_in,
                                      const asn1::rrc_nr::serving_cell_cfg_common_sib_s& sib_in,
                                      subcarrier_spacing                                 ssb_scs);

} // namespace srsgnb

#endif // SRSGNB_SSB_CONFIGURATION_ASN1_HELPERS_H
