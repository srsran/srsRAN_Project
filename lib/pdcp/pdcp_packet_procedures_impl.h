#ifndef SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_IMPL_H
#define SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_IMPL_H

/// Notice that we are not using any implementation specific header files. We are decoupled.
#include "srsgnb/pdcp/pdcp.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/pdcp/pdcp_packet_procedures.h"
#include <cstdio>

/// This class implementation is decoupled from other concrete class implementations as it only depend on public
/// interfaces.

namespace srsgnb {

/// The idea is to have an object to handle each specific procedure specified in the TS.
/// This class would implement the Receive operation procedure of the data transfer category (TS 38.323 5.2.2).
class pdcp_ul_packet_procedure : public pdcp_packet_procedures
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.

  /// Both dependencies are interfaces and they are at the same or in a higher abstraction layer, conforming to the
  /// clean architecture paradigm.
  std::unique_ptr<pdcp_entity> entity;
  pdcp_packet_notifier&        listener;

public:
  pdcp_ul_packet_procedure(std::unique_ptr<pdcp_entity> entity, pdcp_packet_notifier& listener) :
    entity(std::move(entity)), listener(listener)
  {}

  void handle_ul_data_packet(byte_buffer& data) override
  {
    printf("[PDCP-UL-PKT-PROCEDURE] Received a new data packet of size = %u, performing UL packet procedure\n", (unsigned)data.size());
    printf("[PDCP-UL-PKT-PROCEDURE] Step 1: remove PDCP header\n");
    entity->decapsulate(data);
    printf("[PDCP-UL-PKT-PROCEDURE] UL packet procedure finished, delivering packet to upper layer\n");
    listener.on_new_packet(data);
  }
};

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_IMPL_H
