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

#include "split6_flexible_o_du_low_session.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include <memory>

namespace srsran {
class split6_flexible_o_du_low_metrics_notifier;

/// \brief Split 6 flexible O-DU low session manager.
///
/// This class listens to on start/stop requests from FAPI and creates the flexible O-DU low session.
class split6_flexible_o_du_low_session_manager : public fapi::cell_operation_request_notifier
{
public:
  explicit split6_flexible_o_du_low_session_manager(
      std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory_) :
    odu_low_session_factory(std::move(odu_low_session_factory_))
  {
    srsran_assert(odu_low_session_factory, "Invalid O-DU low session factory");
  }

  // See interface for documentation.
  bool on_start_request(const fapi::fapi_cell_config& config) override;

  // See interface for documentation.
  void on_stop_request() override { flexible_odu_low.reset(); }

private:
  std::unique_ptr<split6_flexible_o_du_low_session_factory> odu_low_session_factory;
  std::unique_ptr<split6_flexible_o_du_low_session>         flexible_odu_low;
};

} // namespace srsran
