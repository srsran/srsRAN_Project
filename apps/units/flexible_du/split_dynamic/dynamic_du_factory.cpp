/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dynamic_du_factory.h"
#include "apps/gnb/gnb_appconfig_translators.h"
#include "apps/services/console_helper.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_factories.h"
#include "apps/units/flexible_du/split_8/ru_sdr_factories.h"
#include "dynamic_du_impl.h"
#include "dynamic_du_translators.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ru/ru_dummy_factory.h"

using namespace srsran;

static du_low_wrapper_config create_du_low_config(const du_high_unit_cell_config&       du_high,
                                                  const du_low_unit_config&             params,
                                                  upper_phy_rg_gateway*                 rg_gateway,
                                                  span<task_executor*>                  dl_executors,
                                                  task_executor*                        pucch_executor,
                                                  task_executor*                        pusch_executor,
                                                  task_executor*                        pusch_decoder_executor,
                                                  task_executor*                        prach_executor,
                                                  task_executor*                        srs_executor,
                                                  task_executor*                        pdsch_codeblock_executor,
                                                  upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier,
                                                  unsigned                              sector_id)
{
  du_low_wrapper_config du_lo_wrap_cfg{};
  du_low_cell_config&   du_lo_cell_cfg = du_lo_wrap_cfg.du_low_cfg.cells.emplace_back();

  du_lo_wrap_cfg.du_low_cfg.logger = &srslog::fetch_basic_logger("DU");

  du_lo_cell_cfg.dl_proc_cfg.ldpc_encoder_type   = "auto";
  du_lo_cell_cfg.dl_proc_cfg.crc_calculator_type = "auto";

  const du_low_unit_expert_threads_config& upper_phy_threads_cfg = params.expert_execution_cfg.threads;

  if ((upper_phy_threads_cfg.pdsch_processor_type == "lite") ||
      ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads == 1))) {
    du_lo_cell_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_lite_configuration>();
  } else if ((upper_phy_threads_cfg.pdsch_processor_type == "concurrent") ||
             ((upper_phy_threads_cfg.pdsch_processor_type == "auto") && (upper_phy_threads_cfg.nof_dl_threads > 1))) {
    pdsch_processor_concurrent_configuration pdsch_proc_config;
    pdsch_proc_config.nof_pdsch_codeblock_threads = upper_phy_threads_cfg.nof_dl_threads;
    pdsch_proc_config.max_nof_simultaneous_pdsch =
        (MAX_UE_PDUS_PER_SLOT + 1) * params.expert_phy_cfg.max_processing_delay_slots;
    pdsch_proc_config.pdsch_codeblock_task_executor = pdsch_codeblock_executor;
    du_lo_cell_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_concurrent_configuration>(pdsch_proc_config);
  } else if (upper_phy_threads_cfg.pdsch_processor_type == "generic") {
    du_lo_cell_cfg.dl_proc_cfg.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  } else {
    srsran_assert(false, "Invalid PDSCH processor type {}.", upper_phy_threads_cfg.pdsch_processor_type);
  }
  du_lo_cell_cfg.dl_proc_cfg.nof_concurrent_threads = upper_phy_threads_cfg.nof_dl_threads;

  du_lo_cell_cfg.upper_phy_cfg = generate_du_low_config(du_high, params, sector_id);
  du_lo_wrap_cfg.prach_ports.push_back(du_high.cell.prach_cfg.ports);

  // Fill the rest with the parameters.
  upper_phy_config& cfg          = du_lo_cell_cfg.upper_phy_cfg;
  cfg.rg_gateway                 = rg_gateway;
  cfg.dl_executors               = std::vector<task_executor*>(dl_executors.begin(), dl_executors.end());
  cfg.pucch_executor             = pucch_executor;
  cfg.pusch_executor             = pusch_executor;
  cfg.pusch_decoder_executor     = pusch_decoder_executor;
  cfg.prach_executor             = prach_executor;
  cfg.srs_executor               = srs_executor;
  cfg.rx_symbol_request_notifier = rx_symbol_request_notifier;
  cfg.crc_calculator_type        = "auto";
  cfg.ldpc_rate_dematcher_type   = "auto";
  cfg.ldpc_decoder_type          = "auto";

  return du_lo_wrap_cfg;
}

std::vector<std::unique_ptr<du_wrapper>>
srsran::make_gnb_dus(const gnb_appconfig&                  gnb_cfg,
                     const dynamic_du_unit_config&         dyn_du_cfg,
                     span<du_cell_config>                  du_cells,
                     worker_manager&                       workers,
                     upper_phy_rg_gateway&                 rg_gateway,
                     upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier,
                     srs_du::f1c_connection_client&        f1c_client_handler,
                     srs_du::f1u_du_gateway&               f1u_gw,
                     timer_manager&                        timer_mng,
                     mac_pcap&                             mac_p,
                     rlc_pcap&                             rlc_p,
                     console_helper&                       console_helper,
                     metrics_log_helper&                   metrics_logger,
                     e2_connection_client&                 e2_client_handler,
                     e2_metric_connector_manager&          e2_metric_connectors,
                     rlc_metrics_notifier&                 rlc_json_metrics,
                     metrics_hub&                          metrics_hub)
{
  const du_high_unit_config& du_high  = dyn_du_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_low   = dyn_du_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = dyn_du_cfg.fapi_cfg;

  // DU cell config
  console_helper.set_cells(du_cells);

  // Set up sources for the DU Scheruler UE metrics and add them to metric hub.
  for (unsigned i = 0; i < du_cells.size(); i++) {
    std::string source_name = "DU " + std::to_string(i);
    auto        source      = std::make_unique<scheduler_ue_metrics_source>(source_name);
    metrics_hub.add_source(std::move(source));

    // Get DU Scheduler UE metrics source pointer.
    scheduler_ue_metrics_source* sched_source = metrics_hub.get_scheduler_ue_metrics_source(source_name);
    if (sched_source == nullptr) {
      continue;
    }

    // Connect Console Aggregator to DU Scheduler UE metrics.
    sched_source->add_subscriber(console_helper.get_stdout_metrics_notifier());

    if (metrics_logger.is_enabled()) {
      sched_source->add_subscriber(metrics_logger);
    }

    // Connect JSON metrics reporter to DU Scheduler UE metrics.
    if (gnb_cfg.metrics_cfg.enable_json_metrics) {
      sched_source->add_subscriber(console_helper.get_json_metrics_notifier());
    }

    // Connect E2 agent to DU Scheduler UE metrics.
    if (gnb_cfg.e2_cfg.enable_du_e2) {
      sched_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
    }
  }

  std::vector<std::unique_ptr<du_wrapper>> du_insts;
  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create a gNB config with one cell.
    du_high_unit_config tmp_cfg = du_high;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_high.cells_cfg[i];

    du_wrapper_config           du_cfg = {};
    std::vector<task_executor*> du_low_dl_exec;
    workers.get_du_low_dl_executors(du_low_dl_exec, i);

    // DU-low configuration.
    du_cfg.du_low_cfg = create_du_low_config(tmp_cfg.cells_cfg[i],
                                             du_low,
                                             &rg_gateway,
                                             du_low_dl_exec,
                                             workers.upper_pucch_exec[i],
                                             workers.upper_pusch_exec[i],
                                             workers.upper_pusch_decoder_exec[i],
                                             workers.upper_prach_exec[i],
                                             workers.upper_srs_exec[i],
                                             workers.upper_pdsch_exec[i],
                                             &rx_symbol_request_notifier,
                                             i);

    // DU-high configuration.
    srs_du::du_high_configuration& du_hi_cfg = du_cfg.du_high_cfg.du_hi;
    du_hi_cfg.exec_mapper                    = &workers.get_du_high_executor_mapper(i);
    du_hi_cfg.f1c_client                     = &f1c_client_handler;
    du_hi_cfg.f1u_gw                         = &f1u_gw;
    du_hi_cfg.phy_adapter                    = nullptr;
    du_hi_cfg.timers                         = &timer_mng;
    du_hi_cfg.cells                          = {du_cells[i]};
    du_hi_cfg.srbs                           = generate_du_srb_config(du_high);
    du_hi_cfg.qos                            = generate_du_qos_config(du_high);
    du_hi_cfg.mac_p                          = &mac_p;
    du_hi_cfg.rlc_p                          = &rlc_p;
    du_hi_cfg.gnb_du_id                      = du_insts.size() + 1;
    du_hi_cfg.gnb_du_name                    = fmt::format("srsdu{}", du_hi_cfg.gnb_du_id);
    du_hi_cfg.du_bind_addr =
        transport_layer_address::create_from_string(fmt::format("127.0.0.{}", du_hi_cfg.gnb_du_id));
    du_hi_cfg.mac_cfg = generate_mac_expert_config(du_high);
    // Assign different initial C-RNTIs to different DUs.
    du_hi_cfg.mac_cfg.initial_crnti     = to_rnti(0x4601 + (0x1000 * du_insts.size()));
    du_hi_cfg.sched_ue_metrics_notifier = metrics_hub.get_scheduler_ue_metrics_source("DU " + std::to_string(i));
    du_hi_cfg.sched_cfg                 = generate_scheduler_expert_config(du_high);

    // Connect RLC metrics to sinks, if required
    if (du_high.metrics.rlc.json_enabled || gnb_cfg.e2_cfg.enable_du_e2) {
      // This source aggregates the RLC metrics from all DRBs in a single DU.
      std::string source_name = "rlc_metric_aggr_du_" + std::to_string(i);
      auto        rlc_source  = std::make_unique<rlc_metrics_source>(source_name);

      if (du_high.metrics.rlc.json_enabled) {
        // Connect JSON metrics plotter to RLC metric source.
        rlc_source->add_subscriber(rlc_json_metrics);
      }
      if (metrics_logger.is_enabled()) {
        rlc_source->add_subscriber(metrics_logger);
      }
      if (gnb_cfg.e2_cfg.enable_du_e2) {
        // Connect E2 agent to RLC metric source.
        du_hi_cfg.e2_client          = &e2_client_handler;
        du_hi_cfg.e2ap_config        = generate_e2_config(du_high);
        du_hi_cfg.e2_du_metric_iface = &(e2_metric_connectors.get_e2_du_metrics_interface(i));
        rlc_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
      }
      // Pass RLC metric source to the DU high.
      metrics_hub.add_source(std::move(rlc_source));
      du_hi_cfg.rlc_metrics_notif = metrics_hub.get_rlc_metrics_source(source_name);
    }

    // Configure test mode
    if (du_high.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
      du_hi_cfg.test_cfg.test_ue =
          srs_du::du_test_config::test_ue_config{du_high.test_mode_cfg.test_ue.rnti,
                                                 du_high.test_mode_cfg.test_ue.nof_ues,
                                                 du_high.test_mode_cfg.test_ue.auto_ack_indication_delay,
                                                 du_high.test_mode_cfg.test_ue.pdsch_active,
                                                 du_high.test_mode_cfg.test_ue.pusch_active,
                                                 du_high.test_mode_cfg.test_ue.cqi,
                                                 du_high.test_mode_cfg.test_ue.ri,
                                                 du_high.test_mode_cfg.test_ue.pmi,
                                                 du_high.test_mode_cfg.test_ue.i_1_1,
                                                 du_high.test_mode_cfg.test_ue.i_1_3,
                                                 du_high.test_mode_cfg.test_ue.i_2};
    }

    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    du_cfg.du_high_cfg.fapi.sector    = i;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      du_cfg.du_high_cfg.fapi.executor.emplace(workers.fapi_exec[i]);
    } else {
      report_error_if_not(workers.fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    // As the temporal configuration contains only once cell, pick the data from that cell.
    du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;

    du_insts.push_back(make_du_wrapper(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  return du_insts;
}

static std::unique_ptr<radio_unit> create_dummy_radio_unit(const ru_dummy_unit_config& ru_cfg,
                                                           unsigned                    max_processing_delay_slots,
                                                           unsigned                    nof_prach_ports,
                                                           worker_manager&             workers,
                                                           span<const du_cell_config>  du_cells,
                                                           ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                           ru_timing_notifier&                 timing_notifier,
                                                           ru_error_notifier&                  error_notifier)
{
  ru_dummy_dependencies dependencies;
  dependencies.logger          = &srslog::fetch_basic_logger("RU");
  dependencies.executor        = workers.radio_exec;
  dependencies.timing_notifier = &timing_notifier;
  dependencies.symbol_notifier = &symbol_notifier;

  return create_dummy_ru(generate_ru_dummy_config(ru_cfg, du_cells, max_processing_delay_slots, nof_prach_ports),
                         dependencies);
}

static std::unique_ptr<radio_unit>
create_radio_unit(const variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
                  worker_manager&                                                                     workers,
                  span<const du_cell_config>                                                          du_cells,
                  ru_uplink_plane_rx_symbol_notifier&                                                 symbol_notifier,
                  ru_timing_notifier&                                                                 timing_notifier,
                  ru_error_notifier&                                                                  error_notifier,
                  unsigned max_processing_delay,
                  unsigned prach_nof_ports)
{
  if (variant_holds_alternative<ru_ofh_unit_parsed_config>(ru_cfg)) {
    ru_ofh_factory_config config;
    config.ru_cfg                     = variant_get<ru_ofh_unit_parsed_config>(ru_cfg).config;
    config.max_processing_delay_slots = max_processing_delay;
    config.du_cells                   = du_cells;

    ru_ofh_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_ofh_radio_unit(config, dependencies);
  }

  if (variant_holds_alternative<ru_sdr_unit_config>(ru_cfg)) {
    ru_sdr_factory_config config;
    config.du_cells                   = du_cells;
    config.ru_cfg                     = variant_get<ru_sdr_unit_config>(ru_cfg);
    config.max_processing_delay_slots = max_processing_delay;

    ru_sdr_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_sdr_radio_unit(config, dependencies);
  }

  return create_dummy_radio_unit(variant_get<ru_dummy_unit_config>(ru_cfg),
                                 max_processing_delay,
                                 prach_nof_ports,
                                 workers,
                                 du_cells,
                                 symbol_notifier,
                                 timing_notifier,
                                 error_notifier);
}

static std::vector<du_cell_config> generate_du_cell_config_temp()
{
  std::vector<du_cell_config> out;

  return out;
}

std::unique_ptr<du> srsran::create_du_wrapper(const dynamic_du_unit_config&        dyn_du_cfg,
                                              worker_manager&                      workers,
                                              srs_du::f1c_connection_client&       f1c_client_handler,
                                              srs_du::f1u_du_gateway&              f1u_gw,
                                              timer_manager&                       timer_mng,
                                              mac_pcap&                            mac_p,
                                              rlc_pcap&                            rlc_p,
                                              console_helper&                      console_helper,
                                              e2_connection_client&                e2_client_handler,
                                              e2_metric_connector_manager&         e2_metric_connectors,
                                              rlc_metrics_notifier&                rlc_json_metrics,
                                              metrics_hub&                         metrics_hub,
                                              span<scheduler_ue_metrics_notifier*> sched_metrics_subscribers)
{
  auto du_cells = generate_du_cell_config_temp();

  // DU cell config
  console_helper.set_cells(du_cells);

  // Set up sources for the DU Scheruler UE metrics and add them to metric hub.
  for (unsigned i = 0; i < du_cells.size(); i++) {
    std::string source_name = "DU " + std::to_string(i);
    auto        source      = std::make_unique<scheduler_ue_metrics_source>(source_name);
    for (auto* subscriber : sched_metrics_subscribers) {
      source->add_subscriber(*subscriber);
    }

    metrics_hub.add_source(std::move(source));
  }

  const du_high_unit_config& du_hi    = dyn_du_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = dyn_du_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = dyn_du_cfg.fapi_cfg;

  std::vector<std::unique_ptr<du_wrapper>> du_insts;
  auto                                     du_impl = std::make_unique<dynamic_du_impl>(du_cells.size());

  std::vector<cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>               max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  du_wrapper_config du_cfg = {};
  make_du_low_wrapper_config_and_dependencies(du_cfg.du_low_cfg,
                                              du_lo,
                                              std::move(prach_ports),
                                              du_cells,
                                              max_pusch_per_slot,
                                              du_impl->get_upper_ru_dl_rg_adapter(),
                                              du_impl->get_upper_ru_ul_request_adapter(),
                                              workers);

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create a gNB config with one cell.
    du_high_unit_config tmp_cfg = du_hi;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_hi.cells_cfg[i];

    // DU-high configuration.
    srs_du::du_high_configuration& du_hi_cfg = du_cfg.du_high_cfg.du_hi;
    du_hi_cfg.exec_mapper                    = &workers.get_du_high_executor_mapper(i);
    du_hi_cfg.f1c_client                     = &f1c_client_handler;
    du_hi_cfg.f1u_gw                         = &f1u_gw;
    du_hi_cfg.phy_adapter                    = nullptr;
    du_hi_cfg.timers                         = &timer_mng;
    du_hi_cfg.cells                          = {du_cells[i]};
    du_hi_cfg.srbs                           = generate_du_srb_config(du_hi);
    du_hi_cfg.qos                            = generate_du_qos_config(du_hi);
    du_hi_cfg.mac_p                          = &mac_p;
    du_hi_cfg.rlc_p                          = &rlc_p;
    du_hi_cfg.gnb_du_id                      = du_insts.size() + 1;
    du_hi_cfg.gnb_du_name                    = fmt::format("srsdu{}", du_hi_cfg.gnb_du_id);
    du_hi_cfg.du_bind_addr =
        transport_layer_address::create_from_string(fmt::format("127.0.0.{}", du_hi_cfg.gnb_du_id));
    du_hi_cfg.mac_cfg = generate_mac_expert_config(du_hi);
    // Assign different initial C-RNTIs to different DUs.
    du_hi_cfg.mac_cfg.initial_crnti     = to_rnti(0x4601 + (0x1000 * du_insts.size()));
    du_hi_cfg.sched_ue_metrics_notifier = metrics_hub.get_scheduler_ue_metrics_source("DU " + std::to_string(i));
    du_hi_cfg.sched_cfg                 = generate_scheduler_expert_config(du_hi);

    // Connect RLC metrics to sinks, if required
    if (du_hi.metrics.rlc.json_enabled || du_hi.e2_cfg.enable_du_e2) {
      // This source aggregates the RLC metrics from all DRBs in a single DU.
      std::string source_name = "rlc_metric_aggr_du_" + std::to_string(i);
      auto        rlc_source  = std::make_unique<rlc_metrics_source>(source_name);

      if (du_hi.metrics.rlc.json_enabled) {
        // Connect JSON metrics plotter to RLC metric source.
        rlc_source->add_subscriber(rlc_json_metrics);
      }
      if (du_hi.e2_cfg.enable_du_e2) {
        // Connect E2 agent to RLC metric source.
        du_hi_cfg.e2_client          = &e2_client_handler;
        du_hi_cfg.e2ap_config        = generate_e2_config(du_hi);
        du_hi_cfg.e2_du_metric_iface = &(e2_metric_connectors.get_e2_du_metrics_interface(i));
        rlc_source->add_subscriber(e2_metric_connectors.get_e2_du_metric_notifier(i));
      }
      // Pass RLC metric source to the DU high.
      metrics_hub.add_source(std::move(rlc_source));
      du_hi_cfg.rlc_metrics_notif = metrics_hub.get_rlc_metrics_source(source_name);
    }

    // Configure test mode
    if (du_hi.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
      du_hi_cfg.test_cfg.test_ue =
          srs_du::du_test_config::test_ue_config{du_hi.test_mode_cfg.test_ue.rnti,
                                                 du_hi.test_mode_cfg.test_ue.nof_ues,
                                                 du_hi.test_mode_cfg.test_ue.auto_ack_indication_delay,
                                                 du_hi.test_mode_cfg.test_ue.pdsch_active,
                                                 du_hi.test_mode_cfg.test_ue.pusch_active,
                                                 du_hi.test_mode_cfg.test_ue.cqi,
                                                 du_hi.test_mode_cfg.test_ue.ri,
                                                 du_hi.test_mode_cfg.test_ue.pmi,
                                                 du_hi.test_mode_cfg.test_ue.i_1_1,
                                                 du_hi.test_mode_cfg.test_ue.i_1_3,
                                                 du_hi.test_mode_cfg.test_ue.i_2};
    }
    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    du_cfg.du_high_cfg.fapi.sector    = i;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      du_cfg.du_high_cfg.fapi.executor.emplace(workers.fapi_exec[i]);
    } else {
      report_error_if_not(workers.fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    // As the temporal configuration contains only once cell, pick the data from that cell.
    du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;

    du_insts.push_back(make_du_wrapper(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  du_impl->add_ru(create_radio_unit(dyn_du_cfg.ru_cfg,
                                    workers,
                                    du_cells,
                                    du_impl->get_upper_ru_ul_adapter(),
                                    du_impl->get_upper_ru_timing_adapter(),
                                    du_impl->get_upper_ru_error_adapter(),
                                    du_lo.expert_phy_cfg.max_processing_delay_slots,
                                    du_hi.cells_cfg.front().cell.prach_cfg.ports.size()));

  du_impl->add_dus(std::move(du_insts));

  return du_impl;
}
