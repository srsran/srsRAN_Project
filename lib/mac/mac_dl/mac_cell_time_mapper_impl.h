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

#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_cell_slot_handler.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Class that provides mapping between slot point and time point.
class mac_cell_time_mapper_impl : public mac_cell_slot_handler, public mac_cell_time_mapper
{
  using usecs = std::chrono::microseconds;

public:
  explicit mac_cell_time_mapper_impl(unsigned numerology_);

  void handle_slot_indication(const mac_cell_timing_context& context) override;
  void handle_error_indication(slot_point sl_tx, error_event event) override {}

  std::optional<mac_cell_slot_time_info> get_last_mapping() const override;
  std::optional<time_point>              get_time_point(slot_point slot) const override;
  std::optional<slot_point>              get_slot_point(time_point time) const override;

private:
  srslog::basic_logger&                                   logger;
  mutable lockfree_triple_buffer<mac_cell_timing_context> cur_slot_time_pair;

  const unsigned         numerology;
  const unsigned         nof_slots_per_sf;
  const usecs            slot_dur;
  static constexpr usecs frame_dur = usecs{10000};
};

} // namespace srsran
