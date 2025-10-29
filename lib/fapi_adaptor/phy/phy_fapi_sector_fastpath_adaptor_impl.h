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

#include "srsran/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_fastpath_adaptor.h"
#include <memory>

namespace srsran {
class uplink_request_processor;

namespace fapi_adaptor {

/// PHY-FAPI bidirectional sector fastpath adaptor implementation.
class phy_fapi_sector_fastpath_adaptor_impl : public phy_fapi_sector_fastpath_adaptor
{
public:
  explicit phy_fapi_sector_fastpath_adaptor_impl(std::unique_ptr<phy_fapi_p7_sector_fastpath_adaptor> p7_adaptor_);

  // See interface for documentation.
  phy_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() override;

private:
  std::unique_ptr<phy_fapi_p7_sector_fastpath_adaptor> p7_adaptor;
};

} // namespace fapi_adaptor
} // namespace srsran
