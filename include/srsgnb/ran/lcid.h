
#ifndef SRSGNB_RAN_LCID_H
#define SRSGNB_RAN_LCID_H

#include <cstdint>

namespace srsgnb {

using lcid_t = uint16_t;

constexpr lcid_t MAX_LCID = 11;

enum class srb_id_t : uint16_t { srb0 = 0, srb1, srb2, srb3, nulltype };

inline bool is_srb(lcid_t lcid)
{
  return lcid < static_cast<lcid_t>(srb_id_t::nulltype);
}

inline bool is_lcid_valid(lcid_t lcid)
{
  return lcid <= MAX_LCID;
}
inline srb_id_t to_srb_id(lcid_t lcid)
{
  return is_srb(lcid) ? (srb_id_t)lcid : srb_id_t::nulltype;
}

} // namespace srsgnb

#endif // SRSGNB_RAN_LCID_H
