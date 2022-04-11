
#ifndef SRSGNB_RAN_RNTI_H
#define SRSGNB_RAN_RNTI_H

#include <cstdint>

namespace srsgnb {

/// \remark See TS 38.331 - RNTI-Value and TS 38.321, Table 7.1-1. Values: (0..65535)
enum rnti_t : uint16_t {
  NA_RNTI   = 0x0,
  MIN_CRNTI = 0x1,
  // ...
  MAX_CRNTI         = 0xFFEF,
  MIN_RESERVED_RNTI = 0xFFF0,
  // ...
  MAX_RESERVED_RNTI = 0xFFFD,
  P_RNTI            = 0xFFFE,
  SI_RNTI           = 0xFFFF
};

/// \remark See TS 38.321, Table 7.1-1: RNTI Values.
constexpr rnti_t INVALID_RNTI = rnti_t::NA_RNTI;

/// Checks whether C-RNTI is within 0x1 and 0xFFEF.
inline bool is_crnti(rnti_t rnti)
{
  return rnti <= rnti_t::MAX_CRNTI and rnti >= rnti_t::MIN_CRNTI;
}

} // namespace srsgnb

#endif // SRSGNB_RAN_RNTI_H
