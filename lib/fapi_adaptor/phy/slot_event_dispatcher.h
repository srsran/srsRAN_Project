/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_PHY_SLOT_EVENT_DISPATCHER_H
#define SRSGNB_LIB_FAPI_ADAPTOR_PHY_SLOT_EVENT_DISPATCHER_H

#include "phy_to_fapi_translator.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"

namespace srsgnb {
namespace fapi_adaptor {

class phy_to_fapi_translator;
class fapi_to_phy_translator;

/// This class listens to the \c on_tti_boundary event and dispatches it to multiple clients.
class slot_event_dispatcher : public upper_phy_timing_notifier
{
public:
  slot_event_dispatcher(phy_to_fapi_translator& phy_translator, fapi_to_phy_translator& fapi_translator);

  // See interface for documentation.
  void on_tti_boundary(slot_point slot) override;

private:
  phy_to_fapi_translator& phy_translator;
  fapi_to_phy_translator& fapi_translator;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_PHY_SLOT_EVENT_DISPATCHER_H
