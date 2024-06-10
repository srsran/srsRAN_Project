/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/du/du_wrapper_factory.h"
#include "du_wrapper_impl.h"
#include "srsran/du/du_high_wrapper_factory.h"
#include "srsran/du_low/du_low_wrapper_factory.h"

using namespace srsran;

std::unique_ptr<du_wrapper> srsran::make_du_wrapper(const du_wrapper_config& du_cfg)
{
  du_wrapper_impl_dependencies dependencies;

  // Create DU low.
  dependencies.du_lo = make_du_low_wrapper(du_cfg.du_low_cfg, du_cfg.du_high_cfg.du_hi.cells);

  // Create DU high wrapper.
  du_high_wrapper_dependencies high_wrapper_dependencies;
  for (unsigned i = 0, e = du_cfg.du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    high_wrapper_dependencies.sectors.push_back(
        {&dependencies.du_lo->get_slot_message_gateway(i), &dependencies.du_lo->get_slot_last_message_notifier(i)});
  }

  dependencies.du_hi = make_du_high_wrapper(du_cfg.du_high_cfg, std::move(high_wrapper_dependencies));

  for (unsigned i = 0, e = du_cfg.du_low_cfg.prach_ports.size(); i != e; ++i) {
    dependencies.du_lo->set_slot_time_message_notifier(i, dependencies.du_hi->get_slot_time_message_notifier(i));
    dependencies.du_lo->set_slot_error_message_notifier(i, dependencies.du_hi->get_slot_error_message_notifier(i));
    dependencies.du_lo->set_slot_data_message_notifier(i, dependencies.du_hi->get_slot_data_message_notifier(i));
  }

  srslog::fetch_basic_logger("DU").info("DU wrapper created successfully");

  return std::make_unique<du_wrapper_impl>(std::move(dependencies));
}
