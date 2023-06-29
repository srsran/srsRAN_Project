/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_du_factory.h"
#include "gnb_appconfig_translators.h"
#include "phy_factory.h"
#include "srsran/du/du_factory.h"

using namespace srsran;

std::unique_ptr<du> srsran::make_gnb_du(const gnb_appconfig&                  gnb_cfg,
                                        worker_manager&                       workers,
                                        const std::vector<du_cell_config>&    du_cells,
                                        upper_phy_rg_gateway&                 rg_gateway,
                                        upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
                                        f1ap_message_notifier&                f1ap_pdu_notifier,
                                        srs_du::f1u_du_gateway&               f1u_gw,
                                        timer_manager&                        timer_mng,
                                        mac_pcap&                             mac_p,
                                        scheduler_ue_metrics_notifier&        metrics_notifier)
{
  // DU QoS config
  std::map<five_qi_t, du_qos_config> du_qos_cfg = generate_du_qos_config(gnb_cfg);

  du_config du_cfg = {};
  // DU-low configuration.
  du_cfg.du_lo = create_du_low_config(gnb_cfg,
                                      &rg_gateway,
                                      workers.upper_dl_exec.get(),
                                      workers.upper_pucch_exec.get(),
                                      workers.upper_pusch_exec.get(),
                                      workers.upper_prach_exec.get(),
                                      workers.upper_pdsch_exec.get(),
                                      &rx_symbol_request_notifier);
  // DU-high configuration.
  srs_du::du_high_configuration& du_hi_cfg = du_cfg.du_hi;
  du_hi_cfg.exec_mapper                    = workers.du_high_exec_mapper.get();
  du_hi_cfg.f1ap_notifier                  = &f1ap_pdu_notifier;
  du_hi_cfg.f1u_gw                         = &f1u_gw;
  du_hi_cfg.phy_adapter                    = nullptr;
  du_hi_cfg.timers                         = &timer_mng;
  du_hi_cfg.cells                          = du_cells;
  du_hi_cfg.qos                            = du_qos_cfg;
  du_hi_cfg.pcap                           = &mac_p;
  du_hi_cfg.mac_cfg                        = generate_mac_expert_config(gnb_cfg);
  du_hi_cfg.metrics_notifier               = &metrics_notifier;
  du_hi_cfg.sched_cfg                      = generate_scheduler_expert_config(gnb_cfg);
  if (gnb_cfg.test_mode_cfg.test_ue.rnti != INVALID_RNTI) {
    du_hi_cfg.test_cfg.test_ue = srs_du::du_test_config::test_ue_config{gnb_cfg.test_mode_cfg.test_ue.rnti,
                                                                        gnb_cfg.test_mode_cfg.test_ue.pdsch_active,
                                                                        gnb_cfg.test_mode_cfg.test_ue.pusch_active,
                                                                        gnb_cfg.test_mode_cfg.test_ue.cqi,
                                                                        gnb_cfg.test_mode_cfg.test_ue.pmi,
                                                                        gnb_cfg.test_mode_cfg.test_ue.ri};
  }
  // FAPI configuration.
  du_cfg.fapi.log_level = gnb_cfg.log_cfg.fapi_level;

  return make_du(du_cfg);
}