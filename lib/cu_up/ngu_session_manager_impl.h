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

#include "ngu_session_manager.h"

namespace srsran::srs_cu_up {

class ngu_session_manager_impl : public ngu_session_manager
{
public:
  ~ngu_session_manager_impl() override = default;
  explicit ngu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& ngu_gws_);

  gtpu_tnl_pdu_session& get_next_ngu_gateway() override;

private:
  const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& ngu_gws;
  uint32_t                                                  next_gw = 0;
};

} // namespace srsran::srs_cu_up
