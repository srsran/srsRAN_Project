#ifndef SRSGNB_F1_INTERFACE_F1_IMPL_H
#define SRSGNB_F1_INTERFACE_F1_IMPL_H

/// Notice that we are not using any implementation specific header files. We are decoupled.
#include "srsgnb/F1_interface/F1u.h"
#include "srsgnb/F1_interface/F1u_packet_procedures.h"
#include <cstdio>
#include <memory>

namespace srsgnb {

class f1u_packet_handler : public F1u_input_gateway
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.
  std::unique_ptr<f1u_packet_procedures> procedure;

public:
  explicit f1u_packet_handler(std::unique_ptr<f1u_packet_procedures> procedure) : procedure(std::move(procedure)) {}

  void handle(byte_buffer& data) override
  {
    std::printf("[F1u-PKT-HANDLER] F1u entry point has received a new data packet of size = %u bytes, passing to the UL procedure\n",
                (unsigned)data.size());

    procedure->handle_ul_data_packet(data);
  }
};

} // namespace srsgnb

#endif // SRSGNB_F1_INTERFACE_F1_IMPL_H
