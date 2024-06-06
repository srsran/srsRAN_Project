/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
      return std::make_unique<rlc_tm_entity>(msg.gnb_du_id,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.tm,
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
      return std::make_unique<rlc_um_entity>(msg.gnb_du_id,
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
      return std::make_unique<rlc_am_entity>(msg.gnb_du_id,
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
