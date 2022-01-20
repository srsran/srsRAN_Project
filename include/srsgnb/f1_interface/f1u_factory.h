#ifndef SRSGNB_F1_INTERFACE_F1U_FACTORY_H
#define SRSGNB_F1_INTERFACE_F1U_FACTORY_H

#include "srsgnb/f1_interface/f1u.h"
#include <memory>

/// This factory header file depends on the F1u interfaces (see above include list). It is kept separate as clients of
/// the F1u interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of an F1u interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1u_input_gateway> create_f1u_interface(f1u_packet_notifier& listener);

} // namespace srsgnb

#endif // SRSGNB_F1_INTERFACE_F1U_FACTORY_H
