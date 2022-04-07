
#ifndef SRSGNB_RAN_RNTI_H
#define SRSGNB_RAN_RNTI_H

#include <cstdint>

namespace srsgnb {

/// \remark See TS 38.331 - RNTI-Value. Values: (0..65535)
using rnti_t = uint16_t;

/// \remark See TS 38.321, Table 7.1-1: RNTI Values.
constexpr rnti_t INVALID_RNTI = 0;
enum class rnti_values : rnti_t {
  NA        = 0x0,
  MIN_CRNTI = 0x1,
  // ...
  MAX_CRNTI    = 0xFFEF,
  MIN_RESERVED = 0xFFF0,
  // ...
  MAX_RESERVED = 0xFFFD,
  P_RNTI       = 0xFFFE,
  SI_RNTI      = 0xFFFF
};

/// Checks whether C-RNTI is within 0x1 and 0xFFEF.
inline bool is_crnti(rnti_t rnti)
{
  return rnti <= static_cast<rnti_t>(rnti_values::MAX_CRNTI) and rnti > 0;
}

} // namespace srsgnb

#endif // SRSGNB_RAN_RNTI_H
