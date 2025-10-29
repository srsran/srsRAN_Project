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

class phy_fapi_p7_sector_fastpath_adaptor;

/// \brief PHY-FAPI bidirectional sector adaptor interface.
///
/// This adaptor represents a sector in FAPI, containing P5 (control/configuration) and P7 (user/slot) FAPI data planes.
class phy_fapi_sector_fastpath_adaptor
{
public:
  virtual ~phy_fapi_sector_fastpath_adaptor() = default;

  // :TODO: return p5 adaptor.

  /// Returns the P7 sector adaptor of this PHY-FAPI sector adaptor.
  virtual phy_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
