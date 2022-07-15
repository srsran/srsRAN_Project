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

namespace srsgnb {

/// This interface represents the data entry point of the transmitting side of a PDCP entity.
/// The upper-layers will use this call to pass RLC SDUs into the TX entity.
class pdcp_tx_sdu_handler
{
public:
  pdcp_tx_sdu_handler()                                       = default;
  virtual ~pdcp_tx_sdu_handler()                              = default;
  pdcp_tx_sdu_handler(const pdcp_tx_sdu_handler&)             = delete;
  pdcp_tx_sdu_handler& operator=(const pdcp_tx_sdu_handler&)  = delete;
  pdcp_tx_sdu_handler(const pdcp_tx_sdu_handler&&)            = delete;
  pdcp_tx_sdu_handler& operator=(const pdcp_tx_sdu_handler&&) = delete;

  /// Handle the incoming SDU.
  virtual void handle_sdu(byte_buffer sdu) = 0;
};

/// This interface represents the data exit point of the transmitting side of a PDCP entity.
/// The PDCP will push PDUs to the lower layers using this interface.
/// The PDCP will also use this interface to order the lower layer to discard PDUs if necessary.
class pdcp_tx_lower_layer_data_notifier
{
public:
  virtual ~pdcp_tx_lower_layer_data_notifier()                                            = default;
  pdcp_tx_lower_layer_data_notifier(const pdcp_tx_lower_layer_data_notifier&)             = delete;
  pdcp_tx_lower_layer_data_notifier& operator=(const pdcp_tx_lower_layer_data_notifier&)  = delete;
  pdcp_tx_lower_layer_data_notifier(const pdcp_tx_lower_layer_data_notifier&&)            = delete;
  pdcp_tx_lower_layer_data_notifier& operator=(const pdcp_tx_lower_layer_data_notifier&&) = delete;

  virtual void on_new_pdu(byte_buffer pdu)    = 0; ///> Pass PDU to the lower layers.
  virtual void on_discard_pdu(uint32_t count) = 0; ///> Order lower layers to discard PDU
};

/// This interface represents the control upper layer that the
/// TX PDCP bearer must notify in case of reaching max HFN,
/// so that keys can be re-negotiated. Other protocol failures
/// will also be notified through this interface.
class pdcp_tx_upper_layer_control_notifier
{
public:
  virtual ~pdcp_tx_upper_layer_control_notifier() = default;

  virtual void on_protocol_failure() = 0;
  virtual void on_max_hfn_reached()  = 0;
};

/// This interface represents the interface through which
/// the lower layers will notify the PDCP transmitting entity
/// of relevant events, namely that it can stop the discard timer.
/// On RLC AM, this is done when a PDCP SDU is ACK'ed, on UM
/// it is when transmission of a PDCP SDU begins.
class pdcp_tx_lower_layer_data_handler
{
public:
  virtual ~pdcp_tx_lower_layer_data_handler() = default;

  virtual void stop_discard_timer(uint32_t count) = 0;
};

/// This interface represents the data entry point of the receiving side of a PDCP entity.
/// The lower-layers will use this class to pass PDUs into the PDCP.
class pdcp_rx_pdu_handler
{
public:
  pdcp_rx_pdu_handler()                                       = default;
  virtual ~pdcp_rx_pdu_handler()                              = default;
  pdcp_rx_pdu_handler(const pdcp_rx_pdu_handler&)             = delete;
  pdcp_rx_pdu_handler& operator=(const pdcp_rx_pdu_handler&)  = delete;
  pdcp_rx_pdu_handler(const pdcp_rx_pdu_handler&&)            = delete;
  pdcp_rx_pdu_handler& operator=(const pdcp_rx_pdu_handler&&) = delete;

  virtual void handle_pdu(byte_buffer pdu) = 0; ///> Handle the incoming PDU.
};

/// This interface represents the data exit point of the receiving side of a PDCP entity.
/// The PDCP will use this class to pass SDUs to the upper-layers.
class pdcp_rx_upper_layer_data_notifier
{
public:
  pdcp_rx_upper_layer_data_notifier()                                                     = default;
  virtual ~pdcp_rx_upper_layer_data_notifier()                                            = default;
  pdcp_rx_upper_layer_data_notifier(const pdcp_rx_upper_layer_data_notifier&)             = delete;
  pdcp_rx_upper_layer_data_notifier& operator=(const pdcp_rx_upper_layer_data_notifier&)  = delete;
  pdcp_rx_upper_layer_data_notifier(const pdcp_rx_upper_layer_data_notifier&&)            = delete;
  pdcp_rx_upper_layer_data_notifier& operator=(const pdcp_rx_upper_layer_data_notifier&&) = delete;

  /// Pass SDU to higher layers.
  virtual void on_new_sdu(byte_buffer sdu) = 0;
};

/// This interface represents the control upper layer that the
/// RX PDCP bearer must notify in case of integrity errors or protocol failures.
class pdcp_rx_upper_layer_control_notifier
{
public:
  virtual ~pdcp_rx_upper_layer_control_notifier()                                               = default;
  pdcp_rx_upper_layer_control_notifier(const pdcp_rx_upper_layer_control_notifier&)             = delete;
  pdcp_rx_upper_layer_control_notifier& operator=(const pdcp_rx_upper_layer_control_notifier&)  = delete;
  pdcp_rx_upper_layer_control_notifier(const pdcp_rx_upper_layer_control_notifier&&)            = delete;
  pdcp_rx_upper_layer_control_notifier& operator=(const pdcp_rx_upper_layer_control_notifier&&) = delete;

  virtual void on_protocol_failure()  = 0;
  virtual void on_integrity_failure() = 0;
};
} // namespace srsgnb
