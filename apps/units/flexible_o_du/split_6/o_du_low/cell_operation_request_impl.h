/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "split6_flexible_o_du_low_impl.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include <memory>

namespace srsran {

/// \brief Cell operation request handler implementation.
///
/// This class listens to on start/stop requests from FAPI and creates the objects needed to run the slot messages of
/// FAPI.
class cell_operation_request_handler_impl : public fapi::cell_operation_request_notifier
{
public:
  // See interface for documentation.
  bool on_start_request(const fapi::fapi_cell_config& config) override;

  // See interface for documentation.
  void on_stop_request() override { flexible_odu_low.reset(); }

private:
  std::unique_ptr<split6_flexible_o_du_low_impl> flexible_odu_low;
};

} // namespace srsran
