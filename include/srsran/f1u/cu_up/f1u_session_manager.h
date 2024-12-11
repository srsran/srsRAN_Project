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

#include "srsran/f1u/cu_up/f1u_gateway.h"

namespace srsran::srs_cu_up {

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                    = default;
  virtual f1u_cu_up_gateway& get_next_f1u_gateway() = 0;
};

} // namespace srsran::srs_cu_up
