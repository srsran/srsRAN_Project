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

#include "flexible_o_du_metrics_aggregator.h"
#include "srsran/du/du.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/du/o_du.h"
#include "srsran/ru/ru_adapters.h"
#include <memory>

namespace srsran {

class radio_unit;

/// \brief Flexible O-RAN DU implementation.
///
/// One O-RAN DU can handle more than one cell.
class flexible_o_du_impl : public srs_du::du, public du_operation_controller
{
public:
  flexible_o_du_impl(unsigned nof_cells_, flexible_o_du_metrics_notifier* notifier);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  /// Adds the given RU to this flexible O-RAN DU.
  void add_ru(std::unique_ptr<radio_unit> active_ru);

  /// Adds the given DU to this flexible O-RAN DU.
  void add_du(std::unique_ptr<srs_du::o_du> active_du);

  /// Returns the O-DU metrics notifier of this flexible O-DU implementation.
  srs_du::o_du_metrics_notifier& get_o_du_metrics_notifier() { return odu_metrics_handler; }

  /// Getters to the adaptors.
  upper_phy_ru_ul_adapter&         get_upper_ru_ul_adapter() { return ru_ul_adapt; }
  upper_phy_ru_timing_adapter&     get_upper_ru_timing_adapter() { return ru_timing_adapt; }
  upper_phy_ru_error_adapter&      get_upper_ru_error_adapter() { return ru_error_adapt; }
  upper_phy_ru_dl_rg_adapter&      get_upper_ru_dl_rg_adapter() { return ru_dl_rg_adapt; }
  upper_phy_ru_ul_request_adapter& get_upper_ru_ul_request_adapter() { return ru_ul_request_adapt; }

private:
  const unsigned                   nof_cells;
  upper_phy_ru_ul_adapter          ru_ul_adapt;
  upper_phy_ru_timing_adapter      ru_timing_adapt;
  upper_phy_ru_error_adapter       ru_error_adapt;
  flexible_o_du_metrics_aggregator odu_metrics_handler;
  std::unique_ptr<srs_du::o_du>    du;
  std::unique_ptr<radio_unit>      ru;
  upper_phy_ru_dl_rg_adapter       ru_dl_rg_adapt;
  upper_phy_ru_ul_request_adapter  ru_ul_request_adapt;
};

} // namespace srsran
