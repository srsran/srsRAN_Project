#ifndef SRSGNB_PHY_HELPERS_H
#define SRSGNB_PHY_HELPERS_H

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"

namespace srsgnb {

class slot_point;
template <typename T>
class interval;

/// symbol Interval [start, stop).
using symbol_interval = interval<uint8_t>;

/// Get MSG3 Delay.
/// \param[in] ul_cfg_common ASN1 BWP-UplinkCommon struct.
/// \param[in] sl_dci An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] pusch_delay MSG3 delay.
/// \param[out] symbols PUSCH OFDM symbol interval where MSG3 is allocated.
int get_msg3_delay(const asn1::rrc_nr::ul_cfg_common_sib_s& ul_cfg_common,
                   const slot_point&                        sl_dci,
                   unsigned&                                pusch_delay,
                   symbol_interval&                         symbols);

} // namespace srsgnb

#endif // SRSGNB_PHY_HELPERS_H
