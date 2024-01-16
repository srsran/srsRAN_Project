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
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/security/security.h"
#include "srsran/srslog/srslog.h"

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

  virtual void handle_pdu(byte_buffer_chain pdu) = 0; ///< Handle the incoming PDU.
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
  pdcp_rx_upper_control_notifier()                                                  = default;
  virtual ~pdcp_rx_upper_control_notifier()                                         = default;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&)             = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&)  = delete;
  pdcp_rx_upper_control_notifier(const pdcp_rx_upper_control_notifier&&)            = delete;
  pdcp_rx_upper_control_notifier& operator=(const pdcp_rx_upper_control_notifier&&) = delete;

  virtual void on_protocol_failure()  = 0;
  virtual void on_integrity_failure() = 0;
  virtual void on_max_count_reached() = 0;
};

/// This interface represents the control SAP of the receiving side of a PDCP entity.
/// The RRC will use this interface to configure security keys and enable/disable
/// integrity and ciphering.
class pdcp_rx_upper_control_interface
{
public:
  pdcp_rx_upper_control_interface()                                                   = default;
  virtual ~pdcp_rx_upper_control_interface()                                          = default;
  pdcp_rx_upper_control_interface(const pdcp_rx_upper_control_interface&)             = delete;
  pdcp_rx_upper_control_interface& operator=(const pdcp_rx_upper_control_interface&)  = delete;
  pdcp_rx_upper_control_interface(const pdcp_rx_upper_control_interface&&)            = delete;
  pdcp_rx_upper_control_interface& operator=(const pdcp_rx_upper_control_interface&&) = delete;

  /// Handle the incoming security config.
  virtual void configure_security(security::sec_128_as_config sec_cfg)                  = 0;
  virtual void set_integrity_protection(security::integrity_enabled integrity_enabled_) = 0;
  virtual void set_ciphering(security::ciphering_enabled ciphering_enabled_)            = 0;

  /// Trigger re-establishment
  virtual void reestablish(security::sec_128_as_config sec_cfg_) = 0;
};
} // namespace srsran
