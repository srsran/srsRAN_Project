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
#include "srsran/adt/optional.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"

/*
 * This file will hold the interfaces and notifiers for the RLC entity.
 * They follow the following nomenclature:
 *
 *   rlc_{tx/rx}_{lower/upper}_layer_{[control/data]}_{interface/notifier}
 *
 * 1. TX/RX indicates whether the interface is intended for the
 *    TX or RX side of the entity
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Control/Data: indicates whether this interface is necessary for "control"
 *    purposes (e.g., notifying the RRC of max retransmissions, or that we are
 *    near max HFN) or "data" purposes (e.g. handling SDUs).
 *    This distinction is only necessary when interfacing with the upper layers,
 *    and as such, we omit it in the interfaces with the lower layers.
 * 4. Interface/Notifier: whether this is an interface the RLC entity will
 *    inherit or a notifier that the RLC will keep as a member.
 *
 */
namespace srsran {

/****************************************
 * Interfaces/notifiers for upper layers
 ****************************************/
/// Structure used to represent an RLC SDU. An RLC SDU
/// can optionally be accompanied with the corresponding PDCP sequence number (SN)
/// so that RLC AM can notify the PDCP of ACKs, and PDCP can notify RLC AM/UM to discard PDCP PDUs
struct rlc_sdu {
  byte_buffer                           buf = {};
  optional<uint32_t>                    pdcp_sn;
  std::chrono::system_clock::time_point time_of_arrival;
  rlc_sdu() = default;
  rlc_sdu(byte_buffer buf_, optional<uint32_t> pdcp_sn_) : buf(std::move(buf_)), pdcp_sn(pdcp_sn_) {}
};

/// This interface represents the data entry point of the transmitting side of a RLC entity.
/// The upper-layer will use this call to pass RLC SDUs into the TX entity.
/// The upper-layer will also inform RLC of SDUs (e.g. PDCP PDUs) to be discarded.
class rlc_tx_upper_layer_data_interface
{
public:
  rlc_tx_upper_layer_data_interface()                                                     = default;
  virtual ~rlc_tx_upper_layer_data_interface()                                            = default;
  rlc_tx_upper_layer_data_interface(const rlc_tx_upper_layer_data_interface&)             = delete;
  rlc_tx_upper_layer_data_interface& operator=(const rlc_tx_upper_layer_data_interface&)  = delete;
  rlc_tx_upper_layer_data_interface(const rlc_tx_upper_layer_data_interface&&)            = delete;
  rlc_tx_upper_layer_data_interface& operator=(const rlc_tx_upper_layer_data_interface&&) = delete;

  /// \brief Interface for higher layers to pass SDUs into RLC
  /// \param sdu SDU to be handled
  virtual void handle_sdu(rlc_sdu sdu) = 0;

  /// \brief Interface for higher layers to discard SDUs from RLC queue
  /// \param pdcp_sn PDCP sequence number (SN) of the SDU that is to be discarded
  virtual void discard_sdu(uint32_t pdcp_sn) = 0;
};

/// This interface represents the data upper layer that the TX RLC bearer must notify on transmission and/or delivery of
/// SDUs it can stop its discard timer.
///
/// The following events shall be notified:
/// - on transmission of an SDU, i.e. pop from SDU queue
/// - on successful delivery of an SDU (only RLC AM)
class rlc_tx_upper_layer_data_notifier
{
public:
  virtual ~rlc_tx_upper_layer_data_notifier() = default;

  /// \brief Informs upper layer about the highest PDCP PDU sequence number of the PDCP PDU that was transmitted to the
  /// lower layers.
  /// \param max_tx_pdcp_sn Highest transmitted PDCP PDU sequence number.
  virtual void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) = 0;

  /// \brief Informs upper layer about the highest PDCP PDU sequence number of the PDCP PDU that was successfully
  /// delivered in sequence towards the UE.
  /// \param max_deliv_pdcp_sn Highest in a sequence delivered PDCP PDU sequence number.
  virtual void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) = 0;
};

/// This interface represents the control upper layer that the
/// TX RLC bearer must notify in case of protocol errors,
/// or, in the case of AM bearers, maximum retransmissions reached.
class rlc_tx_upper_layer_control_notifier
{
public:
  rlc_tx_upper_layer_control_notifier()                                                       = default;
  virtual ~rlc_tx_upper_layer_control_notifier()                                              = default;
  rlc_tx_upper_layer_control_notifier(const rlc_tx_upper_layer_control_notifier&)             = delete;
  rlc_tx_upper_layer_control_notifier& operator=(const rlc_tx_upper_layer_control_notifier&)  = delete;
  rlc_tx_upper_layer_control_notifier(const rlc_tx_upper_layer_control_notifier&&)            = delete;
  rlc_tx_upper_layer_control_notifier& operator=(const rlc_tx_upper_layer_control_notifier&&) = delete;

  virtual void on_protocol_failure() = 0;
  virtual void on_max_retx()         = 0;
};

/***************************************
 * Interfaces/notifiers for lower layers
 ***************************************/
/// This interface represents the data exit point of the transmitting side of a RLC entity.
/// The lower layers will use this interface to pull a PDU from the RLC, or to
/// query the current buffer state of the RLC bearer.
class rlc_tx_lower_layer_interface
{
public:
  rlc_tx_lower_layer_interface()                                                = default;
  virtual ~rlc_tx_lower_layer_interface()                                       = default;
  rlc_tx_lower_layer_interface(const rlc_tx_lower_layer_interface&)             = delete;
  rlc_tx_lower_layer_interface& operator=(const rlc_tx_lower_layer_interface&)  = delete;
  rlc_tx_lower_layer_interface(const rlc_tx_lower_layer_interface&&)            = delete;
  rlc_tx_lower_layer_interface& operator=(const rlc_tx_lower_layer_interface&&) = delete;

  /// \brief Pulls a PDU from the lower end of the RLC TX entity
  /// No PDU is written if the size of \c rlc_pdu_buf is insufficient or the TX buffer is empty.
  /// \param rlc_pdu_buf TX buffer where to encode an RLC Tx PDU. The encoded PDU size cannot exceed the size of the
  /// buffer.
  /// \return Number of bytes taken by the written RLC PDU.
  virtual size_t pull_pdu(span<uint8_t> rlc_pdu_buf) = 0;

  /// \brief Get the buffer status information
  /// This function provides the current buffer state of the RLC TX entity.
  /// This is the gross total size required to fully flush the TX entity (potentially by multiple calls to pull_pdu).
  /// \return Provides the current buffer state
  virtual uint32_t get_buffer_state() = 0;
};

class rlc_tx_lower_layer_notifier
{
public:
  rlc_tx_lower_layer_notifier()                                               = default;
  virtual ~rlc_tx_lower_layer_notifier()                                      = default;
  rlc_tx_lower_layer_notifier(const rlc_tx_lower_layer_notifier&)             = delete;
  rlc_tx_lower_layer_notifier& operator=(const rlc_tx_lower_layer_notifier&)  = delete;
  rlc_tx_lower_layer_notifier(const rlc_tx_lower_layer_notifier&&)            = delete;
  rlc_tx_lower_layer_notifier& operator=(const rlc_tx_lower_layer_notifier&&) = delete;

  /// \brief Method called by RLC bearer whenever its buffer state is updated and the respective result
  /// needs to be forwarded to lower layers.
  virtual void on_buffer_state_update(unsigned bsr) = 0;
};
} // namespace srsran
