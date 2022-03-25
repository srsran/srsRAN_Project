#ifndef SRSGNB_PDCP_PDCP_IMPL_H
#define SRSGNB_PDCP_PDCP_IMPL_H

#include "srsgnb/pdcp/pdcp.h"
#include "srsgnb/pdcp/pdcp_packet_procedures.h"
#include <cstdio>

namespace srsgnb {

class pdcp_procedure_dispatcher : public pdcp_pdu_handler
{
  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
  /// different class. For simplicity, we keep it this way for now.
  /// This is important as the dependency with f1u_packet_procedures interface would eventually disappear, although it
  /// is an interface and not a concrete class the less dependencies the better.
  std::unique_ptr<pdcp_packet_procedures> procedure;

public:
  explicit pdcp_procedure_dispatcher(std::unique_ptr<pdcp_packet_procedures> procedure) :
    procedure(std::move(procedure))
  {}

  void handle_pdu(byte_buffer data) override
  {
    std::printf("[PDCP-PROCEDURE-DISPATCHER] Received a new data packet of size = %u bytes. Dispatching an UL packet "
                "data procedure to handle it\n",
                (unsigned)data.length());

    procedure->handle_ul_data_packet(data);
  }
};

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_IMPL_H
