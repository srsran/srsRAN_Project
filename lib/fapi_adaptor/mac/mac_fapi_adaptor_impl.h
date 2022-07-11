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

#include "fapi_to_mac_translator.h"
#include "mac_to_fapi_translator.h"
#include "slot_event_dispatcher.h"
#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief MAC-FAPI adaptor implementation.
///
/// This class owns all the components of the MAC-FAPI adaptor, handling the objects life. It gives access to all the
/// required interfaces and setters, so the adaptor can be connected with the other modules.
class mac_fapi_adaptor_impl : public mac_fapi_adaptor
{
public:
  mac_fapi_adaptor_impl(fapi::slot_message_gateway& gateway, unsigned sector_id_, subcarrier_spacing scs);

  // See interface for documentation.
  fapi::slot_message_notifier& get_slot_notifier() override;

  // See interface for documentation.
  mac_cell_result_notifier& get_cell_result_notifier() override;

  // See interface for documentation.
  void set_mac_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override;

private:
  const unsigned         sector_id;
  mac_to_fapi_translator mac_translator;
  slot_event_dispatcher  slot_dispatcher;
  fapi_to_mac_translator fapi_translator;
};

} // namespace fapi_adaptor
} // namespace srsgnb
