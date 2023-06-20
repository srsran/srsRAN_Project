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

#include "../../ran/gnb_format.h"
#include "rrc_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

void rrc_ue_impl::on_ue_delete_request(const cause_t& cause)
{
  // FIXME: this enqueues a new CORO on top of an existing one.
  rrc_ue_context_release_command msg = {};
  msg.ue_index                       = context.ue_index;
  msg.cause                          = cause;
  du_processor_notifier.on_ue_context_release_command(msg);
}