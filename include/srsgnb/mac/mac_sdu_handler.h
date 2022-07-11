
#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"

namespace srsgnb {

/// This interface notifies to upper layers the reception of new SDUs over a logical channel.
class mac_sdu_rx_notifier
{
public:
  virtual ~mac_sdu_rx_notifier() = default;

  /// This callback is invoked on each received SDU.
  virtual void on_new_sdu(byte_buffer_slice sdu) = 0;
};

/// This interface represents the entry point of a logical channel in the MAC layer.
class mac_sdu_tx_builder
{
public:
  virtual ~mac_sdu_tx_builder()                                     = default;
  virtual byte_buffer_slice_chain on_new_tx_sdu(unsigned nof_bytes) = 0;
};

} // namespace srsgnb
