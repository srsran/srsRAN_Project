/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "resource_grid_test_doubles.h"

namespace srsran {

/// Resource grid mapper dummy.
class resource_grid_mapper_dummy : public resource_grid_mapper
{
public:
  void map(resource_grid_writer&          grid,
           const re_buffer_reader<>&      input,
           const re_pattern&              pattern,
           const precoding_configuration& precoding) override
  {
  }

  void map(resource_grid_writer&          grid,
           symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding,
           unsigned                       skip_re) override
  {
  }
};

} // namespace srsran
