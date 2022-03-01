#ifndef SRSGNB_SDAP_SDAP_H
#define SRSGNB_SDAP_SDAP_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

// :TODO: These are the main input/output interfaces to pass data traffic between layers. As the implementation
// progresses on we'll add additional files that include new interfaces for configuration and other responsibilities.
// Please note these interfaces represent a single bearer or SDAP entity.

/// This interface provides the entry point in the receiving side of a SDAP entity that handles PDUs.
class sdap_pdu_handler
{
public:
  virtual ~sdap_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle(byte_buffer& pdu) = 0;
};

/// This interface notifies the generation of SDUs in the receiving side of a SDAP entity.
class sdap_sdu_notifier
{
public:
  virtual ~sdap_sdu_notifier() = default;

  /// This callback is invoked on each generated SDU.
  virtual void on_new_sdu(byte_buffer& sdu) = 0;
};

/// This interface provides the entry point in the transmitting side of a SDAP entity that handles SDUs.
class sdap_sdu_handler
{
public:
  virtual ~sdap_sdu_handler() = default;

  /// Handle the incoming SDU.
  virtual void handle(byte_buffer& sdu) = 0;
};

/// This interface notifies the generation of PDUs in the transmitting side of a SDAP entity.
class sdap_pdu_notifier
{
public:
  virtual ~sdap_pdu_notifier() = default;

  /// This callback is invoked on each generated PDU.
  virtual void on_new_pdu(byte_buffer& pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_H
