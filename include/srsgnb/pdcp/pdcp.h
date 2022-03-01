#ifndef SRSGNB_PDCP_PDCP_H
#define SRSGNB_PDCP_PDCP_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

// :TODO: These are the main input/output interfaces to pass data traffic between layers. As the implementation
// progresses on we'll add additional files that include new interfaces for configuration and other responsibilities.
// Please note these interfaces represent a single bearer or PDCP entity.

/// This interface provides the entry point in the receiving side of a PDCP entity that handles PDUs.
class pdcp_pdu_handler
{
public:
  virtual ~pdcp_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle(byte_buffer& pdu) = 0;
};

/// This interface notifies the generation of SDUs in the receiving side of a PDCP entity.
class pdcp_sdu_notifier
{
public:
  virtual ~pdcp_sdu_notifier() = default;

  /// This callback is invoked on each generated SDU.
  virtual void on_new_sdu(byte_buffer& sdu) = 0;
};

/// This interface provides the entry point in the transmitting side of a PDCP entity that handles SDUs.
class pdcp_sdu_handler
{
public:
  virtual ~pdcp_sdu_handler() = default;

  /// Handle the incoming SDU.
  virtual void handle(byte_buffer& sdu) = 0;
};

/// This interface notifies the generation of PDUs in the transmitting side of a PDCP entity.
class pdcp_pdu_notifier
{
public:
  virtual ~pdcp_pdu_notifier() = default;

  /// This callback is invoked on each generated PDU.
  virtual void on_new_pdu(byte_buffer& pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_H
