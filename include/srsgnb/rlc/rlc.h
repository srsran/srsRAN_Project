/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_H
#define SRSGNB_RLC_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config_messages.h"

namespace srsgnb {

/// This interface represents the data-plane entry point of the receiving side of a RLC entity.
/// The lower-layers will use this class to pass PDUs into the RLC.
class rlc_rx_pdu_handler
{
public:
  virtual ~rlc_rx_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_pdu(byte_buffer pdu) = 0;
};

/// This interface represents the data exit point of the receiving side of a RLC entity.
/// The RLC will use this class to pass SDUs to the upper-layers.
class rlc_rx_upper_layer_data_notifier
{
public:
  virtual ~rlc_rx_upper_layer_data_notifier() = default;

  /// This method is called to pass the SDU to the upper layers
  virtual void pass_sdu(byte_buffer pdu) = 0;
  virtual void notify_ack_received()     = 0;
};

/// This interface represents the data-plane entry point of the receiving side of a RLC entity.
/// The upper-layers will use this call to pass RLC SDUs into the TX entity.
class rlc_tx_sdu_handler
{
public:
  virtual ~rlc_tx_sdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_sdu(byte_buffer pdu) = 0;
};

/// This interface represents the data-plane exit point of the receiving side of a RLC entity.
/// The lower layers will use this interface to pull a PDU from the RLC, or to
/// query the current buffer state of the RLC bearer.
class rlc_tx_pdu_transmitter
{
public:
  virtual ~rlc_tx_pdu_transmitter() = default;

  virtual bool pull_pdu(byte_buffer& pdu, uint32_t nof_bytes) = 0;
  virtual void get_buffer_state(uint32_t& bytes)              = 0;
};

/// This interface represents the control upper layer that the
/// TX RLC bearer must notify in case of protocol errors,
/// or, in the case of AM bearers, maximum retransmissions reached.
class rlc_tx_upper_layer_control_notifier
{
public:
  virtual ~rlc_tx_upper_layer_control_notifier() = default;

  virtual void notify_protocol_failure() = 0;
  virtual void notify_max_retx()         = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_H
