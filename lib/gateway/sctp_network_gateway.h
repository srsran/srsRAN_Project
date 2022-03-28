#ifndef SRSGNB_SCTP_NETWORK_GATEWAY_H
#define SRSGNB_SCTP_NETWORK_GATEWAY_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

class sctp_network_gateway : public network_gateway_data_handler
{
public:
  explicit sctp_network_gateway();
  void handle_pdu(const byte_buffer& pdu) override;

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb

#endif // SRSGNB_SCTP_NETWORK_GATEWAY_H
