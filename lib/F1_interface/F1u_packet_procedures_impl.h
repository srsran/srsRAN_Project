#ifndef SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_IMPL_H
#define SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_IMPL_H

/// Notice that we are not using any implementation specific header files. We are decoupled.
#include "srsgnb/F1_interface/F1u.h"
#include "srsgnb/F1_interface/F1u_packet_procedures.h"
#include <cstdio>

/// This class implementation is decoupled from other concrete class implementations as it only depend on public
/// interfaces.

namespace srsgnb {

class f1u_ul_packet_procedure : public f1u_packet_procedures
{
  F1u_packet_notifier& listener;

public:
  explicit f1u_ul_packet_procedure(F1u_packet_notifier& listener) : listener(listener) {}

  void handle_ul_data_packet(byte_buffer& data) override
  {
    std::printf(
        "[F1u-UL-PKT-PROCEDURE] Received a new data packet of size = %u bytes, performing UL packet procedure\n",
        (unsigned)data.size());
    std::printf("[F1u-UL-PKT-PROCEDURE] UL packet procedure finished, delivering packet to upper layer\n");
    listener.on_new_packet(data);
  }
};

} // namespace srsgnb

#endif // SRSGNB_F1_INTERFACE_F1U_PACKET_PROCEDURES_IMPL_H
