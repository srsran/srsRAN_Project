
#ifndef SRSGNB_MAC_SDU_HANDLER_H
#define SRSGNB_MAC_SDU_HANDLER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_byte_buffer.h" // TODO: We should make this type independent of the RLC to improve decoupling.

namespace srsgnb {

/// This interface notifies to upper layers the reception of new SDUs over a logical channel.
class mac_sdu_rx_notifier
{
public:
  virtual ~mac_sdu_rx_notifier() = default;

  /// This callback is invoked on each received SDU.
  virtual void on_new_sdu(byte_buffer_owning_view sdu) = 0;
};

/// This interface represents the entry point of a logical channel in the MAC layer.
class mac_sdu_tx_builder
{
public:
  virtual ~mac_sdu_tx_builder()                             = default;
  virtual rlc_byte_buffer on_new_tx_sdu(unsigned nof_bytes) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_SDU_HANDLER_H
