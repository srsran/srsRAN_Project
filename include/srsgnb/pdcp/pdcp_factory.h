#ifndef SRSGNB_PDCP_PDCP_FACTORY_H
#define SRSGNB_PDCP_PDCP_FACTORY_H

#include "srsgnb/pdcp/pdcp.h"
#include <memory>

/// This factory header file depends on the PDCP interfaces (see above include list). It is kept separate as clients of
/// the PDCP interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of a PDCP interface.
std::unique_ptr<pdcp_input_gateway> create_pdcp(pdcp_packet_notifier& listener);

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_FACTORY_H
