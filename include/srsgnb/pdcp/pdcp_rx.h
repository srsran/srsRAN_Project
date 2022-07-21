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
#include "srsgnb/pdcp/pdcp_rx.h"

/*
 * This file will hold the interfaces and notifiers for the PDCP entity.
 * They follow the following nomenclature:
 *
 *   pdcp_{tx/rx}_{lower/upper}_{[control/data]}_{interface/notifier}
 *
 * 1. TX/RX indicates whether the interface is intended for the
 *    TX or RX side of the entity
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Control/Data: indicates whether this interface is necessary for "control"
 *    purposes (e.g., notifying the RRC of an integrity failure, or that we are
 *    near max HFN) or "data" purposes (e.g. handling SDUs).
 *    This distinction is only necessary when interfacing with the upper layers,
 *    and as such, we omit it in the interfaces with the lower layers.
 * 4. Interface/Notifier: whether this is an interface the PDCP entity will
 *    inherit or if a notifier that the PDCP will keep as a member.
 *
 */
namespace srsgnb {

/// This interface represents the data entry point of the receiving side of a PDCP entity.
/// The lower-layers will use this class to pass PDUs into the PDCP.
class pdcp_rx_lower_interface
{
public:
  pdcp_rx_lower_interface()                                           = default;
  virtual ~pdcp_rx_lower_interface()                                  = default;
  pdcp_rx_lower_interface(const pdcp_rx_lower_interface&)             = delete;
  pdcp_rx_lower_interface& operator=(const pdcp_rx_lower_interface&)  = delete;
  pdcp_rx_lower_interface(const pdcp_rx_lower_interface&&)            = delete;
  pdcp_rx_lower_interface& operator=(const pdcp_rx_lower_interface&&) = delete;

  virtual void handle_pdu(byte_buffer pdu) = 0; ///< Handle the incoming PDU.
};

/// This interface represents the data exit point of the receiving side of a PDCP entity.
/// The PDCP will use this class to pass SDUs to the upper-layers.
class pdcp_rx_upper_data_notifier
{
public:
  pdcp_rx_upper_data_notifier()                                               = default;
  virtual ~pdcp_rx_upper_data_notifier()                                      = default;
  pdcp_rx_upper_data_notifier(const pdcp_rx_upper_data_notifier&)             = delete;
  pdcp_rx_upper_data_notifier& operator=(const pdcp_rx_upper_data_notifier&)  = delete;
  pdcp_rx_upper_data_notifier(const pdcp_rx_upper_data_notifier&&)            = delete;
  pdcp_rx_upper_data_notifier& operator=(const pdcp_rx_upper_data_notifier&&) = delete;

  /// Pass SDU to higher layers.
  virtual void on_new_sdu(byte_buffer sdu) = 0;
};

/// This interface represents the control upper layer that the
/// RX PDCP bearer must notify in case of integrity errors or protocol failures.
class pdcp_rx_upper_control_notifier
{
public:
  virtual ~pdcp_rx_upper_control_notifier()                                         = default;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&)             = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&)  = delete;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&&)            = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&&) = delete;

  virtual void on_protocol_failure()  = 0;
  virtual void on_integrity_failure() = 0;
};
} // namespace srsgnb
