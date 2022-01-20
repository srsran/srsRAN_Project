#ifndef SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_H
#define SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// Collection of packet related procedures of the F1u interface.
/// This class looks similar to F1u_input_gateway, however clients are not the same. The first is intended for clients
/// of outer modules that need to pass packets into F1u, while this interface is intended to process concrete UL data
/// packets, which may be a subcategory of all packets.
class f1u_packet_procedures
{
public:
  virtual ~f1u_packet_procedures() = default;

  /// Handles the input UL data packet.
  /// We may have other packet categories which could be handled by adding new methods to this interface.
  virtual void handle_ul_data_packet(byte_buffer& data) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_H
