/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/gtpu/gtpu_tunnel_ngu_factory.h"
#include "gtpu_tunnel_impl_ngu.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;

std::unique_ptr<gtpu_tunnel_ngu> srsran::create_gtpu_tunnel_ngu(gtpu_tunnel_ngu_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_ngu_impl>(
      msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper, msg.ue_dl_timer_factory);
}
