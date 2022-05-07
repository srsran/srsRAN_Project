
#ifndef SRSGNB_SCHED_CELL_ASN1_HELPERS_H
#define SRSGNB_SCHED_CELL_ASN1_HELPERS_H

#include "srsgnb/scheduler/scheduler_configurator.h"
#include "ssb_configuration_asn1_helpers.h"

namespace srsgnb {

/// Fills mac cell configuration with parameters in ASN.1 ServingCellConfigCommonSIB
void fill_sched_cell_cfg_from_asn1(sched_cell_configuration_request_message&          out,
                                   const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_sib_in,
                                   const asn1::rrc_nr::mib_s&                         mib_in,
                                   subcarrier_spacing                                 ssb_scs);

} // namespace srsgnb

#endif // SRSGNB_SCHED_CELL_ASN1_HELPERS_H
