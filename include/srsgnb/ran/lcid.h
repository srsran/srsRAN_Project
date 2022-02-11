
#ifndef SRSGNB_LCID_H
#define SRSGNB_LCID_H

#include <cstdint>

namespace srsgnb {

using lcid_t = uint16_t;

constexpr lcid_t MAX_LCID = 11;

enum class srb_id : uint16_t { srb0 = 0, srb1 = 1, srb2 = 2, srb3 = 3, nulltype };

inline bool is_srb(lcid_t lcid)
{
  return lcid < static_cast<lcid_t>(srb_id::nulltype);
}

inline bool is_lcid_valid(lcid_t lcid)
{
  return lcid <= MAX_LCID;
}

} // namespace srsgnb

#endif // SRSGNB_LCID_H
