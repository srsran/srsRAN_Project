#ifndef SRSGNB_PHY_HELPERS_H
#define SRSGNB_PHY_HELPERS_H

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

namespace srsgnb {

class slot_point;
template <typename T>
class interval;

/// symbol Interval [start, stop).
using symbol_interval = interval<uint8_t>;

int get_msg3_delay(const asn1::rrc_nr::ul_cfg_common_sib_s& ul_cfg_common,
                   slot_point                               sl_dci,
                   unsigned&                                pusch_delay,
                   symbol_interval&                         symbols);

} // namespace srsgnb

#endif // SRSGNB_PHY_HELPERS_H
