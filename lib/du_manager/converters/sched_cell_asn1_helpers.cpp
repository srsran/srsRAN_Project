
#include "sched_cell_asn1_helpers.h"

using namespace srsgnb;

void srsgnb::fill_sched_cell_cfg_from_asn1(sched_cell_configuration_request_message&          out,
                                           const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_sib_in,
                                           const asn1::rrc_nr::mib_s&                         mib_in,
                                           subcarrier_spacing                                 ssb_scs)
{
  fill_ssb_configuration_from_asn1(out.ssb_config, mib_in, serv_cell_sib_in, ssb_scs);
}
