#pragma once

#include "srsgnb/adt/byte_buffer.h"

namespace asn1 {
namespace f1ap {

struct f1ap_pdu_c;

}

} // namespace asn1

namespace srsran {

/// Collection of packet related procedures of the F1AP interface.
/// This class looks similar to F1AP_input_gateway, however clients are not the same. The first is intended for clients
/// of outer modules that need to pass packets into F1AP, while this interface is intended to process concrete UL data
/// packets, which may be a subcategory of all packets.
class f1ap_packet_procedures
{
public:
  virtual ~f1ap_packet_procedures() = default;

  /// Handles the input UL data packet.
  /// We may have other packet categories which could be handled by adding new methods to this interface.
  virtual void handle_ul_data_packet(const asn1::f1ap::f1ap_pdu_c& pdu) = 0;
};

} // namespace srsran
