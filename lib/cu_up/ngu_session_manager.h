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

#include "srsran/gtpu/gtpu_gateway.h"

namespace srsran::srs_cu_up {

class ngu_session_manager
{
public:
  virtual ~ngu_session_manager()                       = default;
  virtual gtpu_tnl_pdu_session& get_next_ngu_gateway() = 0;
};

} // namespace srsran::srs_cu_up
