/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_adaptor.h"
#include <memory>

namespace srsran {

namespace fapi {
class slot_last_message_notifier;
class slot_message_gateway;
struct fapi_cell_config;
} // namespace fapi

class ru_controller;

namespace fapi_adaptor {

/// MAC-FAPI P7 sector adaptor abstract factory.
class mac_fapi_p7_sector_adaptor_factory
{
public:
  virtual ~mac_fapi_p7_sector_adaptor_factory() = default;

  /// Creates a MAC-FAPI P7 sector adaptor.
  virtual std::unique_ptr<mac_fapi_p7_sector_adaptor> create(const fapi::fapi_cell_config&     fapi_cfg,
                                                             fapi::slot_message_gateway&       gateway,
                                                             fapi::slot_last_message_notifier& last_msg_notifier,
                                                             ru_controller&                    ru_ctrl) = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
