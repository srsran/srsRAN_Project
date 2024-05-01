/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  for (unsigned i = 0, e = du_cfg.du_low_cfg.prach_ports.size(); i != e; ++i) {
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
