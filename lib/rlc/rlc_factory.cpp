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

#include "srsran/rlc/rlc_factory.h"
#include "rlc_am_entity.h"
#include "rlc_tm_entity.h"
#include "rlc_um_entity.h"

using namespace srsran;

std::unique_ptr<rlc_entity> srsran::create_rlc_entity(const rlc_entity_creation_message& msg)
{
  switch (msg.config.mode) {
    case rlc_mode::tm:
      return std::make_unique<rlc_tm_entity>(msg.du_index,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.metrics_period,
                                             msg.rlc_metrics_notif,
                                             *msg.rx_upper_dn,
                                             *msg.tx_upper_dn,
                                             *msg.tx_upper_cn,
                                             *msg.tx_lower_dn,
                                             *msg.timers,
                                             *msg.pcell_executor,
                                             *msg.ue_executor,
                                             *msg.pcap_writer);
    case rlc_mode::um_unidir_dl:
    case rlc_mode::um_unidir_ul:
    case rlc_mode::um_bidir:
      return std::make_unique<rlc_um_entity>(msg.du_index,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.um,
                                             msg.config.metrics_period,
                                             msg.rlc_metrics_notif,
                                             *msg.rx_upper_dn,
                                             *msg.tx_upper_dn,
                                             *msg.tx_upper_cn,
                                             *msg.tx_lower_dn,
                                             *msg.timers,
                                             *msg.pcell_executor,
                                             *msg.ue_executor,
                                             *msg.pcap_writer);
    case rlc_mode::am:
      return std::make_unique<rlc_am_entity>(msg.du_index,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.am,
                                             msg.config.metrics_period,
                                             msg.rlc_metrics_notif,
                                             *msg.rx_upper_dn,
                                             *msg.tx_upper_dn,
                                             *msg.tx_upper_cn,
                                             *msg.tx_lower_dn,
                                             *msg.timers,
                                             *msg.pcell_executor,
                                             *msg.ue_executor,
                                             *msg.pcap_writer);
    default:
      srsran_terminate("RLC mode not supported.");
  }
  return nullptr;
}
