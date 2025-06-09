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

#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory_config.h"
#include "split6_flexible_o_du_low_impl.h"
#include "split6_o_du_low_unit_config.h"
#include "srsran/fapi/cell_configurator_plugin.h"
#include <memory>

namespace srsran {

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
struct fapi_cell_config;
class config_message_gateway;
} // namespace fapi

struct worker_manager;

/// Split 6 flexible O-DU low abstract factory.
class split6_flexible_o_du_low_factory
{
protected:
  split6_o_du_low_unit_config                unit_config;
  worker_manager&                            workers;
  timer_manager&                             timers;
  split6_flexible_o_du_low_metrics_notifier* notifier;

public:
  split6_flexible_o_du_low_factory(split6_o_du_low_unit_config                unit_config_,
                                   worker_manager&                            workers_,
                                   timer_manager&                             timers_,
                                   split6_flexible_o_du_low_metrics_notifier* notifier_) :
    unit_config(unit_config_), workers(workers_), timers(timers_), notifier(notifier_)
  {
  }

  virtual ~split6_flexible_o_du_low_factory() = default;

  /// Creates the cell configurator.
  virtual std::unique_ptr<fapi::cell_configurator_plugin>
  create_fapi_cell_configurator_plugin(srslog::basic_logger& logger, fapi::config_message_gateway& gateway) = 0;

  /// Creates the O-DU low.
  std::unique_ptr<split6_flexible_o_du_low_impl> create_split6_flexible_o_du_low(const fapi::fapi_cell_config& config);

private:
  /// Creates the split 6 slot configurator plugin.
  virtual std::unique_ptr<fapi::slot_configurator_plugin>
  create_plugin(const fapi::fapi_cell_config&     config,
                fapi::slot_message_gateway&       gateway,
                fapi::slot_last_message_notifier& last_msg_notifier) = 0;

  /// Creates Radio Unit.
  std::unique_ptr<radio_unit> create_radio_unit(split6_flexible_o_du_low_impl& odu_low,
                                                const fapi::fapi_cell_config&  config);

  /// Creates O-DU low.
  o_du_low_unit create_o_du_low(const fapi::fapi_cell_config& config,
                                o_du_low_unit_dependencies&&  odu_low_dependencies);
};

/// Creates a split 6 flexible O-DU low factory.
std::unique_ptr<split6_flexible_o_du_low_factory>
create_split6_flexible_o_du_low_factory(split6_o_du_low_unit_config                unit_config,
                                        worker_manager&                            workers,
                                        timer_manager&                             timers,
                                        split6_flexible_o_du_low_metrics_notifier* notifier_);

} // namespace srsran
