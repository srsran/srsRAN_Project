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
#include "srsran/du/du_factory.h"

using namespace srsran;

static du_low_configuration create_du_low_config(const gnb_appconfig&                  params,
                                                 upper_phy_rg_gateway*                 rg_gateway,
                                                 span<task_executor*>                  dl_executors,
                                                 task_executor*                        pucch_executor,
                                                 task_executor*                        pusch_executor,
                                                 task_executor*                        prach_executor,
                                                 task_executor*                        pdsch_codeblock_executor,
                                                 upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier)
{
  du_low_configuration du_lo_cfg{};

  du_lo_cfg.logger = &srslog::fetch_basic_logger("DU");

  du_lo_cfg.dl_proc_cfg.ldpc_encoder_type             = "auto";
  du_lo_cfg.dl_proc_cfg.crc_calculator_type           = "auto";
  du_lo_cfg.dl_proc_cfg.nof_pdsch_codeblock_threads   = params.expert_phy_cfg.nof_pdsch_threads;
  du_lo_cfg.dl_proc_cfg.pdsch_codeblock_task_executor = pdsch_codeblock_executor;

  du_lo_cfg.upper_phy = generate_du_low_config(params);

  // Fill the rest with the parameters.
  upper_phy_config& cfg          = du_lo_cfg.upper_phy.front();
  cfg.rg_gateway                 = rg_gateway;
  cfg.dl_executors               = dl_executors;
  cfg.pucch_executor             = pucch_executor;
  cfg.pusch_executor             = pusch_executor;
  cfg.prach_executor             = prach_executor;
  cfg.rx_symbol_request_notifier = rx_symbol_request_notifier;
  cfg.crc_calculator_type        = "auto";
  cfg.ldpc_rate_dematcher_type   = "auto";
  cfg.ldpc_decoder_type          = "auto";

  return du_lo_cfg;
}

std::vector<std::unique_ptr<du>> srsran::make_gnb_du(const gnb_appconfig&                  gnb_cfg,
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
  std::vector<std::unique_ptr<du>> du_inst;
  for (const auto& du_cell_cfg : du_cells) {
    // DU QoS config
    std::map<five_qi_t, du_qos_config> du_qos_cfg = generate_du_qos_config(gnb_cfg);

    du_config                   du_cfg = {};
    std::vector<task_executor*> du_low_dl_exec;
    workers.get_du_low_dl_executors(du_low_dl_exec);

    // DU-low configuration.
    du_cfg.du_lo = create_du_low_config(gnb_cfg,
                                        &rg_gateway,
                                        du_low_dl_exec,
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
    du_hi_cfg.cells                          = {du_cell_cfg};
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
                                                                          gnb_cfg.test_mode_cfg.test_ue.ri,
                                                                          gnb_cfg.test_mode_cfg.test_ue.pmi,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_1_1,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_1_3,
                                                                          gnb_cfg.test_mode_cfg.test_ue.i_2};
    }
    // FAPI configuration.
    du_cfg.fapi.log_level = gnb_cfg.log_cfg.fapi_level;

    du_inst.push_back(make_du(du_cfg));
    report_error_if_not(du_inst.back(), "Invalid Distributed Unit");
  }

  return du_inst;
}