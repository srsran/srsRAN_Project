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

#include "ngu_session_manager_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_up;

ngu_session_manager_impl::ngu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& ngu_gws_) :
  ngu_gws(ngu_gws_)
{
  srsran_assert(not ngu_gws.empty(), "NG-U gateways cannot be empty");
}

gtpu_tnl_pdu_session& ngu_session_manager_impl::get_next_ngu_gateway()
{
  uint32_t index = next_gw % ngu_gws.size();
  next_gw++;
  return *ngu_gws[index];
}
