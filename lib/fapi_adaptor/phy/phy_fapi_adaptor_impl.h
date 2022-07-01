/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_IMPL_H
#define SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_IMPL_H

#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_translator.h"
#include "slot_event_dispatcher.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief MAC-FAPI adaptor implementation.
///
/// This class owns all the components of the MAC-FAPI adaptor, handling the objects life. It gives access to all the
/// required interfaces and setters, so the adaptor can be connected with the other modules.
class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  phy_fapi_adaptor_impl(unsigned sector_id, downlink_processor_pool& dl_processor_pool, resource_grid_pool& rg_pool);

  // See interface for documentation.
  upper_phy_timing_notifier& get_upper_phy_timing_notifier() override;

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  void set_slot_message_notifier(fapi::slot_message_notifier& fapi_slot_notifier) override;

private:
  phy_to_fapi_translator phy_translator;
  fapi_to_phy_translator fapi_translator;
  slot_event_dispatcher  slot_dispatcher;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_IMPL_H
