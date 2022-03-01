#ifndef SRSGNB_SDAP_SDAP_ENTITY_H
#define SRSGNB_SDAP_SDAP_ENTITY_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

//: TODO: The sdap_entity interface is a temporary name that if it gets used by the final implementation we'll ned to
//: find a better name to avoid confusion with a SDAP entity. Entity here is used to represent a business domain entity.

/// SDAP entity receiving side. Retrieves an SDAP SDU from a lower layer data PDU.
class sdap_entity
{
public:
  virtual ~sdap_entity() = default;

  /// Decapsulates the SDAP header in the provided PDU converting it into an SDAP SDU.
  virtual bool decapsulate(byte_buffer& data) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_ENTITY_H
