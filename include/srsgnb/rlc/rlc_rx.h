/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
 *    purposes (e.g., notifying the RRC of protocol error) or "data" purposes
 *    (e.g. handling PDUs).
 *    This distinction is only necessary when interfacing with the upper layers,
 *    and as such, we omit it in the interfaces with the lower layers.
 * 4. Interface/Notifier: whether this is an interface the RLC entity will
 *    inherit or a notifier that the RLC will keep as a member.
 *
 */
namespace srsgnb {

/***************************************
 * Interfaces/notifiers for upper layers
 ***************************************/
/// This interface represents the data entry point of the receiving side of a RLC entity.
/// The lower-layers will use this class to pass PDUs into the RLC.
class rlc_rx_lower_layer_interface
{
public:
  rlc_rx_lower_layer_interface()                                                = default;
  virtual ~rlc_rx_lower_layer_interface()                                       = default;
  rlc_rx_lower_layer_interface(const rlc_rx_lower_layer_interface&)             = delete;
  rlc_rx_lower_layer_interface& operator=(const rlc_rx_lower_layer_interface&)  = delete;
  rlc_rx_lower_layer_interface(const rlc_rx_lower_layer_interface&&)            = delete;
  rlc_rx_lower_layer_interface& operator=(const rlc_rx_lower_layer_interface&&) = delete;

  /// Handle the incoming PDU.
  virtual void handle_pdu(byte_buffer_slice pdu) = 0;
};

/***************************************
 * Interfaces/notifiers for lower layers
 ***************************************/
/// This interface represents the data exit point of the receiving side of a RLC entity.
/// The RLC will use this class to pass SDUs to the upper-layers.
class rlc_rx_upper_layer_data_notifier
{
public:
  virtual ~rlc_rx_upper_layer_data_notifier() = default;

  /// This method is called to pass the SDU to the upper layers
  virtual void on_new_sdu(byte_buffer_slice_chain pdu) = 0;
};

} // namespace srsgnb
