#include "srsgnb/pdcp/pdcp_factory.h"
#include "pdcp_impl.h"
#include "pdcp_packet_procedures_impl.h"
#include "srsgnb/pdcp/pdcp_entity_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<pdcp_pdu_handler> srsgnb::create_pdcp(pdcp_sdu_rx_notifier& listener)
{
  auto ul_procedure =
      std::unique_ptr<pdcp_packet_procedures>(new pdcp_ul_packet_procedure(create_pdcp_entity(), listener));

  return std::unique_ptr<pdcp_pdu_handler>(new pdcp_procedure_dispatcher(std::move(ul_procedure)));
}
