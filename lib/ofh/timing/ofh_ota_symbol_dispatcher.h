/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include "srsran/ofh/ofh_timing_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class task_executor;

namespace ofh {

class ota_symbol_dispatcher : public ota_symbol_boundary_notifier
{
public:
  ota_symbol_dispatcher(unsigned                         nof_slot_offset_du_ru_,
                        unsigned                         nof_symbols_per_slot,
                        std::unique_ptr<timing_notifier> timing_notifier_,
                        span<ota_symbol_handler*>        symbol_handlers_);

  // See interface for documentation.
  void on_new_symbol(slot_symbol_point symbol_point) override;

private:
  /// Notifies new slot.
  void notify_new_slot(slot_symbol_point symbol_point);

private:
  const unsigned                   nof_slot_offset_du_ru;
  const unsigned                   half_slot_symbol;
  const unsigned                   full_slot_symbol;
  std::unique_ptr<timing_notifier> time_notifier;
  std::vector<ota_symbol_handler*> symbol_handlers;
  slot_point                       current_slot;
};

} // namespace ofh
} // namespace srsran
