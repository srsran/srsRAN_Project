
#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"

namespace srsran {

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
  virtual ~mac_sdu_tx_builder() = default;

  /// Called by MAC to generate an MAC Tx SDU for the respective logical channel.
  /// \return Generated MAC SDU.
  virtual byte_buffer_chain on_new_tx_sdu(unsigned nof_bytes) = 0;

  /// Called by MAC to obtain the DL BSR  for the respective logical channel.
  virtual unsigned on_buffer_state_update() = 0;
};

} // namespace srsran
