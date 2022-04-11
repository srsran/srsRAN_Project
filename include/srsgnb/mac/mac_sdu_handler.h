
#ifndef SRSGNB_MAC_SDU_HANDLER_H
#define SRSGNB_MAC_SDU_HANDLER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

struct mac_rx_sdu {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  byte_buffer   pdu;
};

/// This interface notifies to upper layers the reception of new SDUs over a logical channel.
class mac_sdu_rx_notifier
{
public:
  virtual ~mac_sdu_rx_notifier() = default;

  /// This callback is invoked on each received SDU.
  virtual void on_new_sdu(mac_rx_sdu sdu) = 0;
};

/// This interface represents the entry point of a logical channel in the MAC layer.
class mac_sdu_tx_builder
{
public:
  virtual ~mac_sdu_tx_builder()                = default;
  virtual void on_new_tx_sdu(byte_buffer& pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_SDU_HANDLER_H
