/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_TO_FAPI_TRANSLATOR_H
#define SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_TO_FAPI_TRANSLATOR_H

#include "srsgnb/fapi/slot_message_notifier.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief This class translates all the messages and events from the PHY to FAPI.
class phy_to_fapi_translator
{
public:
  explicit phy_to_fapi_translator(fapi::slot_message_notifier& notifier) : notifier(notifier) {}

  /// \brief Handles the new slot by creating a SLOT.notification with the given slot and sending it through the
  /// notifier.
  void handle_new_slot(slot_point slot);

private:
  fapi::slot_message_notifier& notifier;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_TO_FAPI_TRANSLATOR_H
