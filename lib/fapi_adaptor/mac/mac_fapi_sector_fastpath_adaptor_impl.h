/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
