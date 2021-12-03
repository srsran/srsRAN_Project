#ifndef SRSGNB_SDAP_SDAP_ENTITY_FACTORY_H
#define SRSGNB_SDAP_SDAP_ENTITY_FACTORY_H

#include "srsgnb/sdap/sdap_entity.h"
#include <memory>

/// This factory header file depends on the SDAP entity interfaces (see above include list). It is kept separate as
/// clients of the SDAP entity interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of a SDAP entity interface.
std::unique_ptr<sdap_entity> create_sdap_entity();

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_ENTITY_FACTORY_H
