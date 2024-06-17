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

#include "srsran/cu_cp/up_context.h"

namespace srsran {

namespace srs_cu_cp {

class up_resource_manager_impl_interface
{
public:
  virtual ~up_resource_manager_impl_interface() = default;

  /// \brief Return up context for transfer.
  virtual up_context get_up_context() = 0;

  /// \brief Set up context from an already existing context.
  virtual void set_up_context(const up_context& ctx) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
