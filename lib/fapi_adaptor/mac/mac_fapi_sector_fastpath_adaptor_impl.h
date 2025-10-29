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

#include "srsran/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/mac/operation_controller.h"
#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

/// MAC-FAPI bidirectional sector fastpath adaptor implementation.
class mac_fapi_sector_fastpath_adaptor_impl : public mac_fapi_sector_fastpath_adaptor, public operation_controller
{
public:
  /// Constructor for the MAC-FAPI bidirectional sector adaptor.
  explicit mac_fapi_sector_fastpath_adaptor_impl(std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> p7_adaptor_);

  // See interface for documentation.
  mac_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() override;

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override {}

  // See interface for documentation.
  void stop() override { p7_adaptor->stop(); }

private:
  std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> p7_adaptor;
};

} // namespace fapi_adaptor
} // namespace srsran
