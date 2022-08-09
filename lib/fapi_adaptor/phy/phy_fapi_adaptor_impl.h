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

#include "fapi_to_phy_translator.h"
#include "phy_to_fapi_translator.h"
#include "slot_event_dispatcher.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor.h"

namespace srsgnb {
class uplink_request_processor;

namespace fapi {
struct prach_config;
struct carrier_config;
}

namespace fapi_adaptor {

/// \brief PHY-FAPI adaptor implementation.
///
/// This class owns all the components of the PHY-FAPI adaptor, managing the objects lifetime. It gives access to all
/// the required interfaces and setters, so the adaptor can be connected with the other modules.
class phy_fapi_adaptor_impl : public phy_fapi_adaptor
{
public:
  phy_fapi_adaptor_impl(unsigned                  sector_id,
                        downlink_processor_pool&  dl_processor_pool,
                        resource_grid_pool&       rg_pool,
                        subcarrier_spacing        scs_common,
                        uplink_request_processor& ul_request_processor,
                        const fapi::prach_config& prach_tlv,const fapi::carrier_config &carrier_tlv);

  // See interface for documentation.
  upper_phy_timing_notifier& get_upper_phy_timing_notifier() override;

  // See interface for documentation.
  fapi::slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  void set_fapi_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_slot_notifier) override;

private:
  phy_to_fapi_translator phy_translator;
  fapi_to_phy_translator fapi_translator;
  slot_event_dispatcher  slot_dispatcher;
};

} // namespace fapi_adaptor
} // namespace srsgnb
