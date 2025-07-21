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
#include "split6_flexible_o_du_low_session.h"
#include "split6_o_du_low_unit_config.h"
#include "srsran/fapi_adaptor/fapi_slot_messages_adaptor_factory.h"

namespace srsran {

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
struct fapi_cell_config;
class config_message_gateway;
} // namespace fapi

struct worker_manager;

/// Split 6 flexible O-DU low session factory.
class split6_flexible_o_du_low_session_factory
{
protected:
  const split6_o_du_low_unit_config                    unit_config;
  worker_manager&                                      workers;
  timer_manager&                                       timers;
  split6_flexible_o_du_low_metrics_notifier*           notifier;
  fapi::error_message_notifier&                        error_notifier;
  std::unique_ptr<fapi::slot_messages_adaptor_factory> slot_messages_adaptor_factory;

public:
  split6_flexible_o_du_low_session_factory(
      split6_o_du_low_unit_config                          unit_config_,
      worker_manager&                                      workers_,
      timer_manager&                                       timers_,
      fapi::error_message_notifier&                        error_notifier_,
      split6_flexible_o_du_low_metrics_notifier*           notifier_,
      std::unique_ptr<fapi::slot_messages_adaptor_factory> slot_messages_adaptor_factory_) :
    unit_config(std::move(unit_config_)),
    workers(workers_),
    timers(timers_),
    notifier(notifier_),
    error_notifier(error_notifier_),
    slot_messages_adaptor_factory(std::move(slot_messages_adaptor_factory_))
  {
    report_error_if_not(slot_messages_adaptor_factory, "Invalid FAPI slot messages adaptor factory");
  }

  /// Creates the split 6 flexible O-DU low session.
  std::unique_ptr<split6_flexible_o_du_low_session> create_o_du_low_session(const fapi::fapi_cell_config& config);

private:
  /// Creates Radio Unit.
  std::unique_ptr<radio_unit> create_radio_unit(split6_flexible_o_du_low_session& odu_low,
                                                const fapi::fapi_cell_config&     config);

  /// Creates O-DU low.
  o_du_low_unit create_o_du_low(const fapi::fapi_cell_config& config,
                                o_du_low_unit_dependencies&&  odu_low_dependencies);
};

} // namespace srsran
