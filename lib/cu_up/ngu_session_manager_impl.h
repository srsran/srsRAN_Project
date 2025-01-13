/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
