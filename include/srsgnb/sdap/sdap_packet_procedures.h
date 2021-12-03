#ifndef SRSGNB_SDAP_SDAP_PACKET_PROCEDURES_H
#define SRSGNB_SDAP_SDAP_PACKET_PROCEDURES_H

#include "srsgnb/ADT/byte_buffer.h"

namespace srsgnb {

/// Collection of packet related procedures of the SDAP interface.
/// This class looks similar to SDAP_input_gateway, however clients are not the same. The first is intended for clients
/// of outer modules that need to pass packets into SDAP, while this interface is intended to process concrete UL data
/// packets, which may be a subcategory of all packets.
class sdap_packet_procedures
{
public:
  virtual ~sdap_packet_procedures() = default;

  /// Handles the input UL data packet.
  /// We may have other packet categories which could be handled by adding new methods to this interface.
  virtual void handle_ul_data_packet(byte_buffer& data) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_PACKET_PROCEDURES_H
