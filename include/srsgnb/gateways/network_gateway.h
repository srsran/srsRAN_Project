#ifndef SRSGNB_NETWORK_GATEWAY_H
#define SRSGNB_NETWORK_GATEWAY_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// Generic network gateway interfaces to connect components to the outside world.

/// Interface to inject PDUs into gateway entity.
class network_gateway_data_handler
{
public:
  virtual ~network_gateway_data_handler() = default;

  /// \brief Handle the incoming PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void handle_pdu(const byte_buffer& pdu) = 0;
};

/// Interface to inform upper layers about reception of new PDUs.
class network_gateway_data_notifier
{
public:
  virtual ~network_gateway_data_notifier() = default;

  /// \brief This callback is invoked on each received PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

/// Interface to inform upper layers about connection drops, etc.
class network_gateway_control_notifier
{
public:
  virtual ~network_gateway_control_notifier() = default;

  /// This callback is invoked when the connection is dropped.
  virtual void on_connection_loss() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_NETWORK_GATEWAY_H
