#ifndef SRSGNB_PDCP_PDCP_ENTITY_H
#define SRSGNB_PDCP_PDCP_ENTITY_H

#include "srsgnb/ADT/byte_buffer.h"

namespace srsgnb {

/// This interface is at the highest abstraction level, as such, it will be the less likely to change as it contains the
/// business domain logic.
class pdcp_entity
{
public:
  virtual ~pdcp_entity() = default;

  /// Removes the PDCP header of the input packet.
  virtual bool decapsulate(byte_buffer& data) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_ENTITY_H
