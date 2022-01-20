#include "srsgnb/f1_interface/f1u_factory.h"
#include "f1u_impl.h"
#include "f1u_packet_procedures_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<f1u_input_gateway> srsgnb::create_f1u_interface(f1u_packet_notifier& listener)
{
  auto ul_procedure = std::unique_ptr<f1u_packet_procedures>(new f1u_ul_packet_procedure(listener));

  return std::unique_ptr<f1u_input_gateway>(new f1u_packet_handler(std::move(ul_procedure)));
}
