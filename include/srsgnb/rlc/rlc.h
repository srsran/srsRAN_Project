/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config_messages.h"

namespace srsgnb {

/// This interface represents the data entry point of the receiving side of a RLC entity.
/// The lower-layers will use this class to pass PDUs into the RLC.
class rlc_rx_pdu_handler
{
public:
  virtual ~rlc_rx_pdu_handler() = default;

  /// Handle the incoming PDU.
  virtual void handle_pdu(byte_buffer_slice pdu) = 0;
};

/// This interface represents the data exit point of the receiving side of a RLC entity.
/// The RLC will use this class to pass SDUs to the upper-layers.
class rlc_rx_upper_layer_data_notifier
{
public:
  virtual ~rlc_rx_upper_layer_data_notifier() = default;

  /// This method is called to pass the SDU to the upper layers
  virtual void on_new_sdu(byte_buffer_slice pdu) = 0;
};

/// Structure used to represent an RLC SDU. An RLC SDU
/// must be accompanied with the corresponding PDCP Count
/// so that RLC AM can notify the PDCP of ACKs
struct rlc_sdu {
  uint32_t          pdcp_count = 0;
  byte_buffer_slice buf        = {};
  rlc_sdu()                    = default;
  rlc_sdu(uint32_t pdcp_count, byte_buffer_slice buf) : pdcp_count(pdcp_count), buf(std::move(buf)) {}
};

/// This interface represents the data entry point of the transmitting side of a RLC entity.
/// The upper-layers will use this call to pass RLC SDUs into the TX entity.
class rlc_tx_sdu_handler
{
public:
  virtual ~rlc_tx_sdu_handler() = default;

  /// \brief Interface for higher layers to pass SDUs into RLC
  /// \param sdu SDU to be handled
  virtual void handle_sdu(rlc_sdu sdu) = 0;

  // TODO: discard_sdu(uint32_t pdcp_count)
};

/// This interface represents the data exit point of the transmitting side of a RLC entity.
/// The lower layers will use this interface to pull a PDU from the RLC, or to
/// query the current buffer state of the RLC bearer.
class rlc_tx_pdu_transmitter
{
public:
  virtual ~rlc_tx_pdu_transmitter() = default;

  /// \brief Pulls a PDU from the lower end of the RLC TX entity
  /// An empty PDU is returned if nof_bytes is insufficient or the TX buffer is empty.
  /// \param nof_bytes Limits the maximum size of the requested PDU.
  /// \return One PDU
  virtual byte_buffer_slice_chain pull_pdu(uint32_t nof_bytes) = 0;

  /// \brief Get the buffer status information
  /// This function provides the current buffer state of the RLC TX entity.
  /// This is the gross total size required to fully flush the TX entity (potentially by multiple calls to pull_pdu).
  /// \return Provides the current buffer state
  virtual uint32_t get_buffer_state() = 0;
};

/// This interface represents the data upper layer that the RLC bearer must notify on successful delivery of an SDU, so
/// it can stop its discard timer. For RLC AM this is on reception of a corresponding ACK; for RLC UM this is the moment
/// the transmission of the SDU begins.
class rlc_tx_upper_layer_data_notifier
{
public:
  virtual ~rlc_tx_upper_layer_data_notifier() = default;

  virtual void on_delivered_sdu(uint32_t pdcp_count) = 0;
};

/// This interface represents the control upper layer that the
/// TX RLC bearer must notify in case of protocol errors,
/// or, in the case of AM bearers, maximum retransmissions reached.
class rlc_tx_upper_layer_control_notifier
{
public:
  virtual ~rlc_tx_upper_layer_control_notifier() = default;

  virtual void on_protocol_failure() = 0;
  virtual void on_max_retx()         = 0;
};

class rlc_tx_buffer_state_update_notifier
{
public:
  virtual ~rlc_tx_buffer_state_update_notifier() = default;

  /// \brief Method called by RLC bearer whenever its buffer state is updated and the respective result
  /// needs to be forwarded to lower layers.
  virtual void on_buffer_state_update(unsigned bsr) = 0;
};

} // namespace srsgnb
