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
#include "srsran/du/du_high/du_high_wrapper_factory.h"
#include "srsran/du/du_low/du_low_wrapper_factory.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<du_wrapper> srsran::srs_du::make_du_wrapper(const du_wrapper_config&  du_cfg,
                                                            du_wrapper_dependencies&& dependencies)
{
  du_wrapper_impl_dependencies wrapper_deps;

  // Create DU low.
  wrapper_deps.du_lo = make_du_low_wrapper(du_cfg.du_low_cfg, du_cfg.du_high_cfg.du_hi.ran.cells);

  // Fill DU high wrapper dependencies.
  srsran_assert(du_cfg.du_low_cfg.du_low_cfg.cells.size() == dependencies.du_high_deps.sectors.size(),
                "DU low number of cells '{}' does not match the number of cells of the DU high dependencies '{}'",
                du_cfg.du_low_cfg.du_low_cfg.cells.size(),
                dependencies.du_high_deps.sectors.size());
  for (unsigned i = 0, e = du_cfg.du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    du_high_wrapper_sector_dependencies& deps = dependencies.du_high_deps.sectors[i];
    deps.gateway                              = &wrapper_deps.du_lo->get_slot_message_gateway(i);
    deps.last_msg_notifier                    = &wrapper_deps.du_lo->get_slot_last_message_notifier(i);
  }

  wrapper_deps.du_hi = make_du_high_wrapper(du_cfg.du_high_cfg, std::move(dependencies.du_high_deps));

  for (unsigned i = 0, e = du_cfg.du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    wrapper_deps.du_lo->set_slot_time_message_notifier(i, wrapper_deps.du_hi->get_slot_time_message_notifier(i));
    wrapper_deps.du_lo->set_slot_error_message_notifier(i, wrapper_deps.du_hi->get_slot_error_message_notifier(i));
    wrapper_deps.du_lo->set_slot_data_message_notifier(i, wrapper_deps.du_hi->get_slot_data_message_notifier(i));
  }

  srslog::fetch_basic_logger("DU").info("DU wrapper created successfully");

  return std::make_unique<du_wrapper_impl>(std::move(wrapper_deps));
}
