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

/*
 * This file will hold the interfaces and notifiers for the GTP-U entity.
 * They follow the following nomenclature:
 *
 *   gtpu_{dl/ul}_{lower/upper}_layer_{interface/notifier}
 *
 * 1. DL/UL indicates whether the interface is intended for the
 *    DL or UL side of the entity. DL/UL terminology is used opposed
 *    to TX/RX, as TX/RX is ambiguous in the GTP-U.
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Interface/Notifier: whether this is an interface the GTP-U entity will
 *    inherit or a notifier that the GTP-U will keep as a member.
 *
 */

namespace srsgnb {

/****************************************
 * Interfaces/notifiers for the gateway
 ****************************************/
/// This interface represents the data entry point of the transmitting side of a RLC entity.
/// The upper-layers will use this call to pass GTP-U SDUs into the TX entity.
class gtpu_dl_upper_layer_interface
{
public:
  gtpu_dl_upper_layer_interface()                                                 = default;
  virtual ~gtpu_dl_upper_layer_interface()                                        = default;
  gtpu_dl_upper_layer_interface(const gtpu_dl_upper_layer_interface&)             = delete;
  gtpu_dl_upper_layer_interface& operator=(const gtpu_dl_upper_layer_interface&)  = delete;
  gtpu_dl_upper_layer_interface(const gtpu_dl_upper_layer_interface&&)            = delete;
  gtpu_dl_upper_layer_interface& operator=(const gtpu_dl_upper_layer_interface&&) = delete;

  /// \brief Interface for the IO gateway to pass SDUs into the GTP-U
  /// \param sdu SDU to be handled
  virtual void handle_sdu(byte_buffer sdu) = 0;
};

} // namespace srsgnb
