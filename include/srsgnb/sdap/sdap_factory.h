#ifndef SRSGNB_SDAP_SDAP_FACTORY_H
#define SRSGNB_SDAP_SDAP_FACTORY_H

#include "srsgnb/sdap/sdap.h"
#include <memory>

/// This factory header file depends on the SDAP interfaces (see above include list). It is kept separate as clients of
/// the SDAP interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of a SDAP interface.
std::unique_ptr<sdap_input_gateway> create_sdap_packet_receiver(sdap_packet_notifier& listener);

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_FACTORY_H
