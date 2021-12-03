#ifndef SRSGNB_PDCP_PDCP_ENTITY_FACTORY_H
#define SRSGNB_PDCP_PDCP_ENTITY_FACTORY_H

#include "srsgnb/pdcp/pdcp_entity.h"
#include <memory>

/// This factory header file depends on the PDCP entity interfaces (see above include list). It is kept separate as
/// clients of the PDCP entity interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of a PDCP entity interface.
std::unique_ptr<pdcp_entity> create_pdcp_entity();

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_ENTITY_FACTORY_H
