/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/ofh_symbol_handler.h"
#include "srsran/ofh/ofh_timing_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

class ota_symbol_dispatcher : public ota_symbol_boundary_notifier
{
public:
  ota_symbol_dispatcher(unsigned                            nof_slot_offset_du_ru_,
                        unsigned                            nof_symbols_per_slot,
                        srslog::basic_logger&               logger_,
                        std::unique_ptr<timing_notifier>    timing_notifier_,
                        span<symbol_handler*>               symbol_handlers_,
                        span<ota_symbol_boundary_notifier*> ota_notifiers_);

  // See interface for documentation.
  void on_new_symbol(slot_point slot, unsigned symbol_index) override;

private:
  const unsigned                             nof_slot_offset_du_ru;
  const unsigned                             half_slot_symbol;
  const unsigned                             full_slot_symbol;
  srslog::basic_logger&                      logger;
  std::unique_ptr<timing_notifier>           time_notifier;
  std::vector<symbol_handler*>               symbol_handlers;
  std::vector<ota_symbol_boundary_notifier*> ota_notifiers;
  slot_point                                 current_slot;
};

} // namespace ofh
} // namespace srsran