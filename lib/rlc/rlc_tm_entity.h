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

#include "rlc_base_entity.h"
#include "rlc_rx_tm_entity.h"
#include "rlc_tx_tm_entity.h"

namespace srsran {

class rlc_tm_entity : public rlc_base_entity
{
public:
  rlc_tm_entity(gnb_du_id_t                          gnb_du_id_,
                du_ue_index_t                        ue_index_,
                rb_id_t                              rb_id_,
                const rlc_tm_config&                 config,
                timer_duration                       metrics_period_,
                rlc_metrics_notifier*                rlc_metrics_notifier_,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_lower_layer_notifier&         tx_lower_dn,
                rlc_pcap&                            pcap,
                task_executor&                       pcell_executor,
                task_executor&                       ue_executor,
                timer_manager&                       timers) :
    rlc_base_entity(gnb_du_id_,
                    ue_index_,
                    rb_id_,
                    metrics_period_,
                    rlc_metrics_notifier_,
                    timer_factory{timers, ue_executor})
  {
    tx = std::make_unique<rlc_tx_tm_entity>(gnb_du_id_,
                                            ue_index_,
                                            rb_id_,
                                            config.tx,
                                            tx_upper_dn,
                                            tx_upper_cn,
                                            tx_lower_dn,
                                            rlc_metrics_notifier_,
                                            metrics_period.count() != 0,
                                            pcap,
                                            pcell_executor,
                                            ue_executor,
                                            timers);
    rx = std::make_unique<rlc_rx_tm_entity>(gnb_du_id_,
                                            ue_index_,
                                            rb_id_,
                                            config.rx,
                                            rx_upper_dn,
                                            rlc_metrics_notifier_,
                                            metrics_period.count() != 0,
                                            pcap,
                                            ue_executor,
                                            timers);
  }
};

} // namespace srsran
