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

class mac_fapi_sector_fastpath_adaptor;

/// MAC-FAPI bidirectional fastpath adaptor interface.
class mac_fapi_fastpath_adaptor
{
public:
  virtual ~mac_fapi_fastpath_adaptor() = default;

  /// Returns the MAC-FAPI sector fastpath adaptor for the given cell id.
  virtual mac_fapi_sector_fastpath_adaptor& get_sector_adaptor(unsigned cell_id) = 0;

  /// Stops the MAC-FAPI adaptor.
  virtual void stop() = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
