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

namespace srsran {
namespace fapi_adaptor {

class phy_fapi_sector_adaptor;

/// PHY&ndash;FAPI bidirectional adaptor interface.
class phy_fapi_adaptor
{
public:
  virtual ~phy_fapi_adaptor() = default;

  /// Returns the PHY-FAPI sector adaptor for the given cell id.
  virtual phy_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
