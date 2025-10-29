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

class mac_fapi_p7_sector_fastpath_adaptor;
class operation_controller;

/// \brief MAC-FAPI bidirectional sector fastpath adaptor interface.
///
/// This adaptor represents a sector in FAPI, containing the operation controller of the sector, P5
/// (control/configuration) and P7 (user/slot) FAPI data planes.
class mac_fapi_sector_fastpath_adaptor
{
public:
  virtual ~mac_fapi_sector_fastpath_adaptor() = default;

  /// Returns the operation controller of this sector adaptor.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the P7 adaptor of this sector fastpath adaptor.
  virtual mac_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
