#ifndef SRSGNB_SDAP_SDAP_IMPL_H
#define SRSGNB_SDAP_SDAP_IMPL_H

#include "srsgnb/sdap/sdap.h"
#include "srsgnb/sdap/sdap_packet_procedures.h"
#include <cstdio>

namespace srsgnb {

class sdap_procedure_dispatcher : public sdap_pdu_handler
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.
  std::unique_ptr<sdap_packet_procedures> procedure;

public:
  explicit sdap_procedure_dispatcher(std::unique_ptr<sdap_packet_procedures> procedure) :
    procedure(std::move(procedure))
  {}

  void handle(byte_buffer& data) override
  {
    std::printf("[SDAP-PROCEDURE-DISPATCHER] Received a new data packet of size = %u bytes. Dispatching an UL packet "
                "data procedure to handle it\n",
                (unsigned)data.size());

    procedure->handle_ul_data_packet(data);
  }
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_IMPL_H
