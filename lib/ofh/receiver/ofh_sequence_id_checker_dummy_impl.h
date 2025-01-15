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

#include "srsran/ofh/receiver/ofh_sequence_id_checker.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sequence identifier checker dummy implementation.
class sequence_id_checker_dummy_impl : public sequence_id_checker
{
public:
  // See interface for documentation.
  int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id) override { return 0; };
};

} // namespace ofh
} // namespace srsran
