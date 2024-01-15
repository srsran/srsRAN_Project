/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_tx_pdu.h"
#include "srsran/security/security.h"

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
namespace srsran {

/// This interface represents the data exit point of the transmitting side of a PDCP entity.
/// The PDCP will push PDUs to the lower layers using this interface.
/// The PDCP will also use this interface to order the lower layer to discard PDUs if necessary.
class pdcp_tx_lower_notifier
{
public:
  pdcp_tx_lower_notifier()                                          = default;
  virtual ~pdcp_tx_lower_notifier()                                 = default;
  pdcp_tx_lower_notifier(const pdcp_tx_lower_notifier&)             = delete;
  pdcp_tx_lower_notifier& operator=(const pdcp_tx_lower_notifier&)  = delete;
  pdcp_tx_lower_notifier(const pdcp_tx_lower_notifier&&)            = delete;
  pdcp_tx_lower_notifier& operator=(const pdcp_tx_lower_notifier&&) = delete;

  virtual void on_new_pdu(pdcp_tx_pdu pdu)      = 0; ///< Pass PDCP PDU to the lower layers.
  virtual void on_discard_pdu(uint32_t pdcp_sn) = 0; ///< Order lower layers to discard PDU
};

/// This interface represents the notification entry point of the transmitting side of a PDCP entity.
/// The lower layers will use this interface to inform the PDCP Tx about transmitted and successfully delivered PDUs.
/// The PDCP uses this information to stop the discard timers of PDUs that are associated with those notifications:
///
/// - RLC AM: stop discard timer for successfully delivered PDUs.
/// - RLC UM: stop discard timer for transmitted PDUs.
class pdcp_tx_lower_interface
{
public:
  pdcp_tx_lower_interface()                                           = default;
  virtual ~pdcp_tx_lower_interface()                                  = default;
  pdcp_tx_lower_interface(const pdcp_tx_lower_interface&)             = delete;
  pdcp_tx_lower_interface& operator=(const pdcp_tx_lower_interface&)  = delete;
  pdcp_tx_lower_interface(const pdcp_tx_lower_interface&&)            = delete;
  pdcp_tx_lower_interface& operator=(const pdcp_tx_lower_interface&&) = delete;

  /// \brief Informs the PDCP entity about the highest PDCP PDU sequence number of the PDCP PDU that was transmitted by
  /// the lower layers (i.e. by the RLC).
  ///
  /// In case of RLC AM, the PDCP will ignore this notification.
  /// In case of RLC UM, the PDCP will stop the discard timers for all PDUs up to highest_sn.
  ///
  /// \param highest_sn Highest transmitted PDCP PDU sequence number.
  virtual void handle_transmit_notification(uint32_t highest_sn) = 0;

  /// \brief Informs the PDCP about the highest PDCP PDU sequence number of the PDCP PDU that was successfully
  /// delivered in sequence towards the UE.
  ///
  /// In case of RLC AM, the PDCP will stop the discard timers for all PDUs up to highest_sn.
  /// In case of RLC UM, the PDCP will ignore this notification.
  ///
  /// \param highest_sn Highest in a sequence delivered PDCP PDU sequence number.
  virtual void handle_delivery_notification(uint32_t highest_sn) = 0;
};

/// This interface represents the data entry point of the transmitting side of a PDCP entity.
/// The upper-layers will use this call to pass PDUs into the TX entity.
class pdcp_tx_upper_data_interface
{
public:
  pdcp_tx_upper_data_interface()                                                = default;
  virtual ~pdcp_tx_upper_data_interface()                                       = default;
  pdcp_tx_upper_data_interface(const pdcp_tx_upper_data_interface&)             = delete;
  pdcp_tx_upper_data_interface& operator=(const pdcp_tx_upper_data_interface&)  = delete;
  pdcp_tx_upper_data_interface(const pdcp_tx_upper_data_interface&&)            = delete;
  pdcp_tx_upper_data_interface& operator=(const pdcp_tx_upper_data_interface&&) = delete;

  /// Handle the incoming SDU.
  virtual void handle_sdu(byte_buffer sdu) = 0;
};

/// This interface represents the control SAP of the transmitting side of a PDCP entity.
/// The RRC will use this interface to configure security keys and enable/disable
/// integrity and ciphering.
class pdcp_tx_upper_control_interface
{
public:
  pdcp_tx_upper_control_interface()                                                   = default;
  virtual ~pdcp_tx_upper_control_interface()                                          = default;
  pdcp_tx_upper_control_interface(const pdcp_tx_upper_control_interface&)             = delete;
  pdcp_tx_upper_control_interface& operator=(const pdcp_tx_upper_control_interface&)  = delete;
  pdcp_tx_upper_control_interface(const pdcp_tx_upper_control_interface&&)            = delete;
  pdcp_tx_upper_control_interface& operator=(const pdcp_tx_upper_control_interface&&) = delete;

  /// Setup security
  virtual void configure_security(security::sec_128_as_config sec_cfg)                  = 0;
  virtual void set_integrity_protection(security::integrity_enabled integrity_enabled_) = 0;
  virtual void set_ciphering(security::ciphering_enabled ciphering_enabled_)            = 0;

  /// Trigger data recovery
  virtual void data_recovery() = 0;

  /// Trigger re-establishment
  virtual void reestablish(security::sec_128_as_config sec_cfg) = 0;
};

/// This interface represents the control upper layer that the
/// TX PDCP bearer must notify in case of reaching max HFN,
/// so that keys can be re-negotiated. Other protocol failures
/// will also be notified through this interface.
class pdcp_tx_upper_control_notifier
{
public:
  pdcp_tx_upper_control_notifier()                                                  = default;
  virtual ~pdcp_tx_upper_control_notifier()                                         = default;
  pdcp_tx_upper_control_notifier(const pdcp_tx_upper_control_notifier&)             = delete;
  pdcp_tx_upper_control_notifier& operator=(const pdcp_tx_upper_control_notifier&)  = delete;
  pdcp_tx_upper_control_notifier(const pdcp_tx_upper_control_notifier&&)            = delete;
  pdcp_tx_upper_control_notifier& operator=(const pdcp_tx_upper_control_notifier&&) = delete;

  virtual void on_protocol_failure()  = 0;
  virtual void on_max_count_reached() = 0;
};
} // namespace srsran
