/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_H
#define SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_H

#include "srsgnb/fapi/slot_message_notifier.h"

namespace srsgnb {

class mac_cell_slot_handler;

namespace fapi_adaptor {

/// \brief Interface to the MAC side of the FAPI adaptor object.
///
/// This interface gives access to the interfaces needed to interconnect the adaptor with the MAC layer.
///
/// \note This object has the ownership of all the components of the adaptor.
class mac_fapi_adaptor
{
public:
  virtual ~mac_fapi_adaptor() = default;

  /// \brief Returns the adaptor's slot_message_notifier.
  virtual fapi::slot_message_notifier& get_slot_notifier() = 0;

  /// \brief Configures the adaptor's MAC cell slot handler to the given one.
  ///
  /// \param[in] mac_slot_handler MAC slot handler to set in the adaptor.
  virtual void set_mac_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) = 0;

  // :TODO: add the rest of the MAC handlers.

  // :TODO: should provide access to the cell interface.
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_H
