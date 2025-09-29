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

#include "srsran/phy/upper/upper_phy_factories.h"
#include "downlink_processor_multi_executor_impl.h"
#include "downlink_processor_pool_impl.h"
#include "metrics/upper_phy_metrics_collector_impl.h"
#include "uplink_processor_impl.h"
#include "uplink_processor_pool_impl.h"
#include "upper_phy_impl.h"
#include "upper_phy_pdu_validators.h"
#include "upper_phy_rx_symbol_handler_printer_decorator.h"
#include "srsran/phy/metrics/phy_metrics_factories.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pdcch/factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pucch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#include "srsran/phy/upper/channel_processors/ssb/factories.h"
#include "srsran/phy/upper/signal_processors/prs/factories.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "srsran/support/error_handling.h"
#include <algorithm>

using namespace srsran;

static std::unique_ptr<downlink_processor_pool>
create_downlink_processor_pool(std::shared_ptr<downlink_processor_factory> factory,
                               const upper_phy_factory_configuration&      factory_config,
                               const upper_phy_configuration&              config,
                               const upper_phy_dependencies&               dependencies);

static std::unique_ptr<resource_grid_pool>
create_dl_resource_grid_pool(const upper_phy_factory_dependencies&  factory_dependencies,
                             const upper_phy_configuration&         config,
                             std::shared_ptr<resource_grid_factory> rg_factory);

static std::shared_ptr<uplink_processor_factory>
create_ul_processor_factory(const upper_phy_factory_configuration& config,
                            const upper_phy_factory_dependencies&  dependencies,
                            std::shared_ptr<resource_grid_factory> rg_factory,
                            upper_phy_metrics_notifiers*           metric_notifier);

static std::unique_ptr<uplink_processor_pool>
create_ul_processor_pool(uplink_processor_factory&              factory,
                         rx_buffer_pool&                        rm_buffer_pool,
                         upper_phy_rx_results_notifier&         rx_results_notifier,
                         const upper_phy_factory_configuration& factory_config,
                         const upper_phy_configuration&         config);

static std::unique_ptr<prach_buffer_pool> create_prach_pool(const upper_phy_configuration& config);

namespace {

class uplink_processor_base_factory : public uplink_processor_factory
{
public:
  uplink_processor_base_factory(std::shared_ptr<pucch_processor_factory>         pucch_factory_,
                                std::shared_ptr<pusch_processor_factory>         pusch_factory_,
                                std::shared_ptr<prach_detector_factory>          prach_factory_,
                                std::shared_ptr<srs_estimator_factory>           srs_factory_,
                                std::shared_ptr<resource_grid_factory>           grid_factory_,
                                uplink_processor_impl::task_executor_collection& task_executors_) :
    pucch_factory(std::move(pucch_factory_)),
    pusch_factory(std::move(pusch_factory_)),
    prach_factory(std::move(prach_factory_)),
    srs_factory(std::move(srs_factory_)),
    grid_factory(std::move(grid_factory_)),
    task_executors(task_executors_)
  {
    report_fatal_error_if_not(prach_factory, "Invalid PRACH factory.");
    report_fatal_error_if_not(pusch_factory, "Invalid PUSCH factory.");
    report_fatal_error_if_not(pucch_factory, "Invalid PUCCH factory.");
    report_fatal_error_if_not(srs_factory, "Invalid SRS factory.");
  }

  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    std::unique_ptr<prach_detector> prach = prach_factory->create();
    report_fatal_error_if_not(prach, "Invalid PRACH detector.");

    std::unique_ptr<pusch_processor> pusch_proc = pusch_factory->create();
    report_fatal_error_if_not(pusch_proc, "Invalid PUSCH processor.");

    std::unique_ptr<pucch_processor> pucch_proc = pucch_factory->create();
    report_fatal_error_if_not(pucch_proc, "Invalid PUCCH processor.");

    std::unique_ptr<srs_estimator> srs = srs_factory->create();
    report_fatal_error_if_not(srs, "Invalid SRS estimator.");

    std::unique_ptr<resource_grid> grid =
        grid_factory->create(config.nof_rx_ports, MAX_NSYMB_PER_SLOT, config.nof_rb * NOF_SUBCARRIERS_PER_RB);
    report_fatal_error_if_not(grid, "Invalid SRS estimator.");

    return std::make_unique<uplink_processor_impl>(std::move(prach),
                                                   std::move(pusch_proc),
                                                   std::move(pucch_proc),
                                                   std::move(srs),
                                                   std::move(grid),
                                                   task_executors,
                                                   config.rm_buffer_pool,
                                                   config.notifier,
                                                   config.nof_rb,
                                                   config.max_nof_layers);
  }

  std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& logger, bool log_all_opportunities) override
  {
    std::unique_ptr<prach_detector> prach = prach_factory->create(logger, log_all_opportunities);
    report_fatal_error_if_not(prach, "Invalid PRACH detector.");

    std::unique_ptr<pusch_processor> pusch_proc = pusch_factory->create(logger);
    report_fatal_error_if_not(pusch_proc, "Invalid PUSCH processor.");

    std::unique_ptr<pucch_processor> pucch_proc = pucch_factory->create(logger);
    report_fatal_error_if_not(pucch_proc, "Invalid PUCCH processor.");

    std::unique_ptr<srs_estimator> srs = srs_factory->create(logger);
    report_fatal_error_if_not(srs, "Invalid SRS estimator.");

    std::unique_ptr<resource_grid> grid =
        grid_factory->create(config.nof_rx_ports, MAX_NSYMB_PER_SLOT, config.nof_rb * NOF_SUBCARRIERS_PER_RB);
    report_fatal_error_if_not(grid, "Invalid SRS estimator.");

    return std::make_unique<uplink_processor_impl>(std::move(prach),
                                                   std::move(pusch_proc),
                                                   std::move(pucch_proc),
                                                   std::move(srs),
                                                   std::move(grid),
                                                   task_executors,
                                                   config.rm_buffer_pool,
                                                   config.notifier,
                                                   config.nof_rb,
                                                   config.max_nof_layers);
  }

  std::unique_ptr<uplink_pdu_validator> create_pdu_validator() override
  {
    return std::make_unique<uplink_processor_validator_impl>(prach_factory->create_validator(),
                                                             pucch_factory->create_validator(),
                                                             pusch_factory->create_validator(),
                                                             srs_factory->create_validator());
  }

private:
  std::shared_ptr<pucch_processor_factory>        pucch_factory;
  std::shared_ptr<pusch_processor_factory>        pusch_factory;
  std::shared_ptr<prach_detector_factory>         prach_factory;
  std::shared_ptr<srs_estimator_factory>          srs_factory;
  std::shared_ptr<resource_grid_factory>          grid_factory;
  uplink_processor_impl::task_executor_collection task_executors;
};

/// \brief Factory to create single executor downlink processors.
class downlink_processor_single_executor_factory : public downlink_processor_factory
{
public:
  struct factory_configuration {
    std::shared_ptr<pdcch_processor_factory>      pdcch_proc_factory;
    std::shared_ptr<pdsch_processor_factory>      pdsch_proc_factory;
    std::shared_ptr<ssb_processor_factory>        ssb_proc_factory;
    std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory;
    std::shared_ptr<prs_generator_factory>        prs_gen_factory;
    task_executor*                                pdcch_executor;
    task_executor*                                pdsch_executor;
    task_executor*                                ssb_executor;
    task_executor*                                csi_rs_executor;
    task_executor*                                prs_executor;
  };

  downlink_processor_single_executor_factory(factory_configuration& config) :
    pdcch_proc_factory(std::move(config.pdcch_proc_factory)),
    pdsch_proc_factory(std::move(config.pdsch_proc_factory)),
    ssb_proc_factory(std::move(config.ssb_proc_factory)),
    nzp_csi_rs_factory(std::move(config.nzp_csi_rs_factory)),
    prs_gen_factory(std::move(config.prs_gen_factory)),
    pdcch_executor(*config.pdcch_executor),
    pdsch_executor(*config.pdsch_executor),
    ssb_executor(*config.ssb_executor),
    csi_rs_executor(*config.csi_rs_executor),
    prs_executor(*config.prs_executor)
  {
    srsran_assert(pdcch_proc_factory, "Invalid PDCCH processor factory.");
    srsran_assert(pdsch_proc_factory, "Invalid PDSCH processor factory.");
    srsran_assert(ssb_proc_factory, "Invalid SSB processor factory.");
    srsran_assert(nzp_csi_rs_factory, "Invalid NZP-CSI-RS generator factory.");
    srsran_assert(prs_gen_factory, "Invalid PRS generator factory.");
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor_base> create(const downlink_processor_config& config) override
  {
    std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create();
    report_fatal_error_if_not(pdcch, "Invalid PDCCH processor.");

    std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create();
    report_fatal_error_if_not(pdsch, "Invalid PDSCH processor.");

    std::unique_ptr<ssb_processor> ssb = ssb_proc_factory->create();
    report_fatal_error_if_not(ssb, "Invalid SSB processor.");

    std::unique_ptr<nzp_csi_rs_generator> nzp_csi = nzp_csi_rs_factory->create();
    report_fatal_error_if_not(nzp_csi, "Invalid NZP-CSI-RS generator.");

    std::unique_ptr<prs_generator> prs_gen = prs_gen_factory->create();
    report_fatal_error_if_not(prs_gen, "Invalid PRS generator.");

    return std::make_unique<downlink_processor_multi_executor_impl>(*config.gateway,
                                                                    std::move(pdcch),
                                                                    std::move(pdsch),
                                                                    std::move(ssb),
                                                                    std::move(nzp_csi),
                                                                    std::move(prs_gen),
                                                                    pdcch_executor,
                                                                    pdsch_executor,
                                                                    ssb_executor,
                                                                    csi_rs_executor,
                                                                    prs_executor,
                                                                    srslog::fetch_basic_logger("PHY"));
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor_base>
  create(const downlink_processor_config& config, srslog::basic_logger& logger, bool enable_broadcast) override
  {
    std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create(logger, enable_broadcast);
    report_fatal_error_if_not(pdcch, "Invalid PDCCH processor.");

    std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create(logger, enable_broadcast);
    report_fatal_error_if_not(pdsch, "Invalid PDSCH processor.");

    std::unique_ptr<ssb_processor> ssb;
    if (enable_broadcast) {
      ssb = ssb_proc_factory->create(logger);
    } else {
      ssb = ssb_proc_factory->create();
    }
    report_fatal_error_if_not(ssb, "Invalid SSB processor.");

    std::unique_ptr<nzp_csi_rs_generator> nzp_csi;
    if (enable_broadcast) {
      nzp_csi = nzp_csi_rs_factory->create(logger);
    } else {
      nzp_csi = nzp_csi_rs_factory->create();
    }
    report_fatal_error_if_not(nzp_csi, "Invalid NZP-CSI-RS generator.");

    std::unique_ptr<prs_generator> prs_gen;
    if (enable_broadcast) {
      prs_gen = prs_gen_factory->create(logger);
    } else {
      prs_gen = prs_gen_factory->create();
    }
    report_fatal_error_if_not(prs_gen, "Invalid PRS generator.");

    return std::make_unique<downlink_processor_multi_executor_impl>(*config.gateway,
                                                                    std::move(pdcch),
                                                                    std::move(pdsch),
                                                                    std::move(ssb),
                                                                    std::move(nzp_csi),
                                                                    std::move(prs_gen),
                                                                    pdcch_executor,
                                                                    pdsch_executor,
                                                                    ssb_executor,
                                                                    csi_rs_executor,
                                                                    prs_executor,
                                                                    srslog::fetch_basic_logger("PHY"));
  }

  std::unique_ptr<downlink_pdu_validator> create_pdu_validator() override
  {
    return std::make_unique<downlink_processor_validator_impl>(ssb_proc_factory->create_validator(),
                                                               pdcch_proc_factory->create_validator(),
                                                               pdsch_proc_factory->create_validator(),
                                                               nzp_csi_rs_factory->create_validator(),
                                                               prs_gen_factory->create_validator());
  }

private:
  std::shared_ptr<pdcch_processor_factory>      pdcch_proc_factory;
  std::shared_ptr<pdsch_processor_factory>      pdsch_proc_factory;
  std::shared_ptr<ssb_processor_factory>        ssb_proc_factory;
  std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory;
  std::shared_ptr<prs_generator_factory>        prs_gen_factory;
  task_executor&                                pdcch_executor;
  task_executor&                                pdsch_executor;
  task_executor&                                ssb_executor;
  task_executor&                                csi_rs_executor;
  task_executor&                                prs_executor;
};

/// Generic upper physical layer receive symbol handler factory.
class upper_phy_rx_symbol_handler_factory_impl : public upper_phy_rx_symbol_handler_factory
{
public:
  // See interface for documentation.
  std::unique_ptr<upper_phy_rx_symbol_handler> create(uplink_slot_processor_pool& ul_processor_pool_) override
  {
    return std::make_unique<upper_phy_rx_symbol_handler_impl>(ul_processor_pool_);
  }
};

/// Upper physical layer receive symbol handler factory with printer decorator.
class upper_phy_rx_symbol_handler_printer_decorator_factory : public upper_phy_rx_symbol_handler_factory
{
public:
  explicit upper_phy_rx_symbol_handler_printer_decorator_factory(
      std::shared_ptr<upper_phy_rx_symbol_handler_factory> factory_,
      srslog::basic_logger&                                logger_,
      const std::string&                                   filename_,
      unsigned                                             nof_rb_,
      interval<unsigned>                                   ul_print_ports_,
      bool                                                 print_prach_) :
    base_factory(std::move(factory_)),
    logger(logger_),
    filename(filename_),
    nof_rb(nof_rb_),
    ul_print_ports(ul_print_ports_),
    print_prach(print_prach_)
  {
    srsran_assert(base_factory, "Invalid Rx symbol handler factory.");
  }

  // See interface for documentation.
  std::unique_ptr<upper_phy_rx_symbol_handler> create(uplink_slot_processor_pool& ul_processor_pool_) override
  {
    // Create the RX symbol handler.
    std::unique_ptr<upper_phy_rx_symbol_handler> rx_symbol_handler = base_factory->create(ul_processor_pool_);

    // Create and return the RX symbol handler printer decorator.
    return std::make_unique<upper_phy_rx_symbol_handler_printer_decorator>(
        std::move(rx_symbol_handler), logger, filename, nof_rb, ul_print_ports, print_prach);
  }

private:
  std::shared_ptr<upper_phy_rx_symbol_handler_factory> base_factory;
  srslog::basic_logger&                                logger;
  const std::string&                                   filename;
  unsigned                                             nof_rb;
  interval<unsigned>                                   ul_print_ports;
  bool                                                 print_prach;
};

class upper_phy_factory_impl : public upper_phy_factory
{
public:
  explicit upper_phy_factory_impl(upper_phy_factory_configuration factory_config_,
                                  upper_phy_factory_dependencies  factory_dependencies_) :
    factory_config(std::move(factory_config_)),
    factory_deps(std::move(factory_dependencies_)),
    rg_factory(create_resource_grid_factory())
  {
    srsran_assert(rg_factory, "Invalid resource grid factory.");

    // Create upper PHY metrics collector.
    upper_phy_metrics_notifiers* metric_notifier = nullptr;
    if (factory_config.enable_metrics) {
      metrics_collector = std::make_shared<upper_phy_metrics_collector_impl>();
      metric_notifier   = metrics_collector.get();
    }

    downlink_processor_factory_sw_config dl_fact_config = {
        .ldpc_encoder_type   = factory_config.ldpc_encoder_type,
        .crc_calculator_type = factory_config.crc_calculator_type,
        .pdsch_processor     = factory_config.pdsch_processor,
        .hw_encoder_factory  = factory_deps.hw_encoder_factory,
        .pdcch_executor      = factory_deps.executors.pdcch_executor,
        .pdsch_executor      = factory_deps.executors.pdsch_executor,
        .ssb_executor        = factory_deps.executors.ssb_executor,
        .csi_rs_executor     = factory_deps.executors.csi_rs_executor,
        .prs_executor        = factory_deps.executors.prs_executor,
    };

    downlink_processor_factory_sw_dependencies dl_fact_deps = {.pdsch_codeblock_executor =
                                                                   factory_deps.executors.pdsch_codeblock_executor,
                                                               .metric_notifier = metric_notifier};

    downlink_proc_factory = create_downlink_processor_factory_sw(dl_fact_config, dl_fact_deps);
    report_fatal_error_if_not(downlink_proc_factory, "Failed to create a DL processor factory.");

    if (metric_notifier) {
      downlink_proc_factory = create_downlink_processor_generator_metric_decorator_factory(
          std::move(downlink_proc_factory), metric_notifier->get_downlink_processor_notifier());
      report_fatal_error_if_not(downlink_proc_factory, "Failed to create a DL processor metric decorator factory.");
    }

    uplink_proc_factory = create_ul_processor_factory(factory_config, factory_deps, rg_factory, metric_notifier);
    report_fatal_error_if_not(downlink_proc_factory, "Failed to create an UL processor factory.");

    rx_symbol_handler_factory = create_rx_symbol_handler_factory();
    report_fatal_error_if_not(rx_symbol_handler_factory, "Invalid Rx symbol handler factory.");

    // If the RX symbol filename is set, create an RX symbol handler printer decorator.
    if (!factory_config.rx_symbol_printer_filename.empty()) {
      interval<unsigned> ul_ports(0, factory_config.nof_rx_ports);
      if (factory_config.rx_symbol_printer_port.has_value()) {
        ul_ports.set(*factory_config.rx_symbol_printer_port, *factory_config.rx_symbol_printer_port + 1);
      }
      // Configure RX symbol handler for printing the resource grid.
      srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY", true);
      rx_symbol_handler_factory =
          create_rx_symbol_handler_printer_decorator_factory(std::move(rx_symbol_handler_factory),
                                                             logger,
                                                             factory_config.rx_symbol_printer_filename,
                                                             factory_config.ul_bw_rb,
                                                             ul_ports,
                                                             factory_config.rx_symbol_printer_prach);
      report_fatal_error_if_not(rx_symbol_handler_factory, "Invalid Rx symbol handler printer decorator factory.");
    }

    // Create the RX symbol handler with the PHY tap decorator.
    if (factory_config.enable_phy_tap) {
      rx_symbol_handler_factory = create_rx_symbol_handler_tap_factory(
          std::move(rx_symbol_handler_factory), factory_config.ul_bw_rb, factory_config.nof_rx_ports);
      report_fatal_error_if_not(rx_symbol_handler_factory, "Invalid Rx symbol handler tap factory.");
    }
  }

  std::unique_ptr<upper_phy> create(const upper_phy_configuration& config, const upper_phy_dependencies& deps) override
  {
    upper_phy_impl_config phy_config;
    phy_config.pusch_max_nof_layers        = config.pusch_max_nof_layers;
    phy_config.log_level                   = factory_config.log_level;
    phy_config.rx_symbol_request_notifier  = deps.rx_symbol_request_notifier;
    phy_config.nof_slots_ul_pdu_repository = config.nof_ul_rg;

    phy_config.dl_rg_pool = create_dl_resource_grid_pool(factory_deps, config, rg_factory);
    report_fatal_error_if_not(phy_config.dl_rg_pool, "Invalid downlink resource grid pool.");

    phy_config.dl_processor_pool = create_downlink_processor_pool(downlink_proc_factory, factory_config, config, deps);
    report_fatal_error_if_not(phy_config.dl_processor_pool, "Invalid downlink processor pool.");

    phy_config.rx_buf_pool = create_rx_buffer_pool(config.rx_buffer_config);
    report_fatal_error_if_not(phy_config.rx_buf_pool, "Invalid receive buffer processor pool.");

    phy_config.rx_results_notifier = std::make_unique<upper_phy_rx_results_notifier_wrapper>();

    phy_config.ul_processor_pool = create_ul_processor_pool(*uplink_proc_factory,
                                                            phy_config.rx_buf_pool->get_pool(),
                                                            *phy_config.rx_results_notifier,
                                                            factory_config,
                                                            config);
    report_fatal_error_if_not(phy_config.ul_processor_pool, "Invalid uplink processor pool.");

    phy_config.prach_pool = create_prach_pool(config);
    report_fatal_error_if_not(phy_config.prach_pool, "Invalid PRACH buffer pool.");

    // Create the validators.
    phy_config.dl_pdu_validator = downlink_proc_factory->create_pdu_validator();
    phy_config.ul_pdu_validator = uplink_proc_factory->create_pdu_validator();

    // Add the metrics collector.
    phy_config.metrics_collector = metrics_collector;

    // Create the RX symbol handler.
    phy_config.rx_symbol_handler =
        rx_symbol_handler_factory->create(phy_config.ul_processor_pool->get_slot_processor_pool());

    return std::make_unique<upper_phy_impl>(std::move(phy_config));
  }

private:
  const upper_phy_factory_configuration                factory_config;
  const upper_phy_factory_dependencies                 factory_deps;
  std::shared_ptr<resource_grid_factory>               rg_factory;
  std::shared_ptr<downlink_processor_factory>          downlink_proc_factory;
  std::shared_ptr<uplink_processor_factory>            uplink_proc_factory;
  std::shared_ptr<upper_phy_metrics_collector_impl>    metrics_collector;
  std::shared_ptr<upper_phy_rx_symbol_handler_factory> rx_symbol_handler_factory;
};

} // namespace

static std::unique_ptr<downlink_processor_pool>
create_downlink_processor_pool(std::shared_ptr<downlink_processor_factory> factory,
                               const upper_phy_factory_configuration&      factory_config,
                               const upper_phy_configuration&              config,
                               const upper_phy_dependencies&               dependencies)
{
  downlink_processor_pool_config config_pool;

  for (unsigned numerology = 0, numerology_end = to_numerology_value(subcarrier_spacing::invalid);
       numerology != numerology_end;
       ++numerology) {
    // Skip SCS if not active.
    if (!config.active_scs[numerology]) {
      continue;
    }

    downlink_processor_pool_config::downlink_processor_set info = {to_subcarrier_spacing(numerology), {}};

    for (unsigned i_proc = 0, nof_procs = config.nof_dl_processors; i_proc != nof_procs; ++i_proc) {
      downlink_processor_config processor_config = {.id = i_proc, .gateway = dependencies.rg_gateway};

      std::unique_ptr<downlink_processor_base> dl_proc;
      if (factory_config.log_level == srslog::basic_levels::none) {
        dl_proc = factory->create(processor_config);
      } else {
        // Fetch logger.
        srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY", true);
        dl_proc = factory->create(processor_config, logger, factory_config.enable_logging_broadcast);
      }
      report_fatal_error_if_not(dl_proc, "Invalid downlink processor.");
      info.procs.push_back(std::move(dl_proc));
    }

    config_pool.dl_processors.push_back(std::move(info));
  }

  return create_dl_processor_pool(std::move(config_pool));
}

static std::unique_ptr<resource_grid_pool>
create_dl_resource_grid_pool(const upper_phy_factory_dependencies&  factory_dependencies,
                             const upper_phy_configuration&         config,
                             std::shared_ptr<resource_grid_factory> rg_factory)
{
  // Configure one pool per upper PHY.
  report_fatal_error_if_not(rg_factory, "Invalid resource grid factory.");
  report_fatal_error_if_not(factory_dependencies.executors.dl_grid_executor.executor != nullptr,
                            "Invalid task executor.");

  // Generate resource grid instances.
  std::vector<std::unique_ptr<resource_grid>> grids(config.nof_dl_rg);
  std::generate(
      grids.begin(), grids.end(), [&rg_factory, nof_tx_ports = config.nof_tx_ports, dl_bw_rb = config.dl_bw_rb]() {
        return rg_factory->create(nof_tx_ports, MAX_NSYMB_PER_SLOT, dl_bw_rb * NRE);
      });

  return create_asynchronous_resource_grid_pool(*factory_dependencies.executors.dl_grid_executor.executor,
                                                std::move(grids));
}

static std::shared_ptr<uplink_processor_factory>
create_ul_processor_factory(const upper_phy_factory_configuration& config,
                            const upper_phy_factory_dependencies&  dependencies,
                            std::shared_ptr<resource_grid_factory> rg_factory,
                            upper_phy_metrics_notifiers*           metric_notifier)
{
  // Verify the PUSCH processor capabilities.
  pusch_processor_phy_capabilities pusch_capabilities = get_pusch_processor_phy_capabilities();
  report_fatal_error_if_not(pusch_capabilities.max_nof_layers >= config.pusch_max_nof_layers,
                            "The configured PUSCH maximum number of layers (i.e., {}) exceeds the maximum PUSCH "
                            "processor capable number of layers (i.e., {}).",
                            pusch_capabilities.max_nof_layers,
                            config.pusch_max_nof_layers);

  channel_equalizer_algorithm_type pusch_equalizer_algorithm_type = channel_equalizer_algorithm_type::zf;
  if (config.pusch_channel_equalizer_algorithm == "mmse") {
    pusch_equalizer_algorithm_type = channel_equalizer_algorithm_type::mmse;
  }

  port_channel_estimator_fd_smoothing_strategy pusch_chan_estimator_fd_strategy =
      port_channel_estimator_fd_smoothing_strategy::filter;
  if (config.pusch_channel_estimator_td_strategy == "none") {
    pusch_chan_estimator_fd_strategy = port_channel_estimator_fd_smoothing_strategy::none;
  } else if (config.pusch_channel_estimator_td_strategy == "mean") {
    pusch_chan_estimator_fd_strategy = port_channel_estimator_fd_smoothing_strategy::mean;
  }

  port_channel_estimator_td_interpolation_strategy pusch_chan_estimator_td_strategy =
      port_channel_estimator_td_interpolation_strategy::average;
  if (config.pusch_channel_estimator_td_strategy == "interpolate") {
    pusch_chan_estimator_td_strategy = port_channel_estimator_td_interpolation_strategy::interpolate;
  }

  std::shared_ptr<low_papr_sequence_generator_factory> sequence_factory =
      create_low_papr_sequence_generator_sw_factory();
  report_fatal_error_if_not(sequence_factory, "Invalid sequence factory.");

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
    report_fatal_error_if_not(dft_factory, "Invalid DFT factory.");
  }

  std::shared_ptr<time_alignment_estimator_factory> ta_est_factory =
      create_time_alignment_estimator_dft_factory(dft_factory);
  report_fatal_error_if_not(ta_est_factory, "Invalid TA estimator factory.");

  std::shared_ptr<prach_generator_factory> prach_gen_factory = create_prach_generator_factory_sw();
  report_fatal_error_if_not(prach_gen_factory, "Invalid PRACH generator factory.");

  std::shared_ptr<prach_detector_factory> prach_factory =
      create_prach_detector_factory_sw(dft_factory, prach_gen_factory);
  report_fatal_error_if_not(prach_factory, "Invalid PRACH detector factory.");

  // Create PRACH detector pool factory if more than one thread is used.
  if (dependencies.executors.prach_executor.max_concurrency > 1) {
    prach_factory = create_prach_detector_pool_factory(std::move(prach_factory),
                                                       dependencies.executors.prach_executor.max_concurrency);
    report_fatal_error_if_not(prach_factory, "Invalid PRACH detector pool factory.");
  }

  std::shared_ptr<srs_estimator_factory> srs_factory =
      create_srs_estimator_generic_factory(sequence_factory, ta_est_factory, config.ul_bw_rb);
  report_fatal_error_if_not(srs_factory, "Invalid SRS estimator factory.");

  // Create SRS estimator pool factory if more than one thread is used.
  if (dependencies.executors.srs_executor.max_concurrency > 1) {
    srs_factory =
        create_srs_estimator_pool(std::move(srs_factory), dependencies.executors.srs_executor.max_concurrency);
    report_fatal_error_if_not(srs_factory, "Invalid SRS estimator pool factory.");
  }

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  report_error_if_not(prg_factory, "Invalid pseudo-random sequence generator factory.");

  // Create port channel estimator for PUSCH and PUCCH.
  std::shared_ptr<port_channel_estimator_factory> pusch_ch_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_est_factory);
  report_error_if_not(prg_factory, "Invalid channel estimator factory.");
  std::shared_ptr<port_channel_estimator_factory> pucch_ch_estimator_factory = pusch_ch_estimator_factory;

  // Wrap PUSCH port channel estimator with metric if necessary.
  if (metric_notifier) {
    // First, wrap TA estimator.
    std::shared_ptr<time_alignment_estimator_factory> metric_ta_est_factory =
        create_time_alignment_estimator_metric_decorator_factory(ta_est_factory,
                                                                 metric_notifier->get_pusch_ta_estimator_notifier());
    report_fatal_error_if_not(metric_ta_est_factory, "Failed to create TA estimator factory.");

    // Create a different factory containing the new TA estimator factory.
    pusch_ch_estimator_factory = create_port_channel_estimator_factory_sw(metric_ta_est_factory);
    report_error_if_not(pusch_ch_estimator_factory, "Invalid channel estimator factory.");

    // Finally, wrap the factory with the metric decorator.
    pusch_ch_estimator_factory = create_port_channel_estimator_metric_decorator_factory(
        std::move(pusch_ch_estimator_factory), metric_notifier->get_pusch_port_channel_estimator_notifier());
    report_error_if_not(pusch_ch_estimator_factory, "Invalid port channel estimator factory.");
  }

  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory =
      create_low_papr_sequence_generator_sw_factory();
  report_error_if_not(low_papr_sequence_gen_factory, "Invalid low-PAPR sequence generator factory.");

  std::shared_ptr<channel_equalizer_factory> pusch_equalizer_factory =
      create_channel_equalizer_generic_factory(pusch_equalizer_algorithm_type);
  report_error_if_not(pusch_equalizer_factory, "Invalid PUSCH equalizer factory.");

  std::shared_ptr<channel_equalizer_factory> pucch_equalizer_factory =
      create_channel_equalizer_generic_factory(channel_equalizer_algorithm_type::zf);
  report_error_if_not(pucch_equalizer_factory, "Invalid PUCCH equalizer factory.");

  std::shared_ptr<transform_precoder_factory> precoding_factory =
      create_dft_transform_precoder_factory(dft_factory, config.ul_bw_rb);
  report_fatal_error_if_not(precoding_factory, "Invalid transform precoder factory.");

  std::shared_ptr<demodulation_mapper_factory> demodulation_factory = create_demodulation_mapper_factory();
  report_error_if_not(demodulation_factory, "Invalid demodulation factory.");

  std::shared_ptr<evm_calculator_factory> evm_calc_factory = create_evm_calculator_factory();
  report_error_if_not(evm_calc_factory, "Invalid EVM calculator factory.");

  std::shared_ptr<crc_calculator_factory> crc_calc_factory =
      create_crc_calculator_factory_sw(config.crc_calculator_type);
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC calculator factory of type {}.", config.crc_calculator_type);

  std::shared_ptr<ulsch_demultiplex_factory> ulsch_demux_factory = create_ulsch_demultiplex_factory_sw();
  report_fatal_error_if_not(ulsch_demux_factory, "Failed to create UL-SCH demultiplex factory.");

  // Create metric notifier decorators.
  std::shared_ptr<crc_calculator_factory>          pusch_crc_calc_factory     = crc_calc_factory;
  std::shared_ptr<pseudo_random_generator_factory> pusch_scrambling_factory   = prg_factory;
  std::shared_ptr<demodulation_mapper_factory>     pusch_demodulation_factory = demodulation_factory;
  std::shared_ptr<evm_calculator_factory>          pusch_evm_calc_factory     = evm_calc_factory;
  std::shared_ptr<transform_precoder_factory>      pusch_precoding_factory    = precoding_factory;
  if (metric_notifier) {
    pusch_scrambling_factory = create_pseudo_random_generator_metric_decorator_factory(
        std::move(pusch_scrambling_factory), metric_notifier->get_pusch_scrambling_notifier());
    report_fatal_error_if_not(pusch_scrambling_factory, "Failed to create factory.");

    pusch_equalizer_factory = create_channel_equalizer_metric_decorator_factory(
        std::move(pusch_equalizer_factory), metric_notifier->get_pusch_channel_equalizer_notifier());
    report_error_if_not(pusch_equalizer_factory, "Failed to create factory.");

    pusch_crc_calc_factory = create_crc_calculator_metric_decorator_factory(
        std::move(pusch_crc_calc_factory), metric_notifier->get_pusch_crc_calculator_notifier());
    report_fatal_error_if_not(pusch_crc_calc_factory, "Invalid PUSCH CRC calculator factory.");

    pusch_demodulation_factory = create_demodulation_mapper_metric_decorator_factory(
        std::move(pusch_demodulation_factory), metric_notifier->get_pusch_demodulation_mapper_notifier());
    report_fatal_error_if_not(pusch_demodulation_factory, "Failed to create factory.");

    pusch_evm_calc_factory = create_evm_calculator_metric_decorator_factory(
        std::move(pusch_evm_calc_factory), metric_notifier->get_pusch_evm_calculator_notifier());
    report_fatal_error_if_not(pusch_evm_calc_factory, "Failed to create factory.");

    ulsch_demux_factory = create_ulsch_demultiplex_metric_decorator_factory(
        std::move(ulsch_demux_factory), metric_notifier->get_ulsch_demultiplex_notifier());
    report_fatal_error_if_not(ulsch_demux_factory, "Failed to create UL-SCH demultiplex factory.");

    pusch_precoding_factory = create_transform_precoder_metric_decorator_factory(
        std::move(pusch_precoding_factory), metric_notifier->get_pusch_transform_precoder_notifier());
    report_fatal_error_if_not(pusch_precoding_factory, "Failed to create transform precoder factory.");
  }

  // If the PUSCH channel estimator is configured for multiple threads, then wrap with a pool.
  if (dependencies.executors.pusch_ch_estimator_executor.max_concurrency > 1) {
    pusch_ch_estimator_factory = create_port_channel_estimator_pool_factory(
        pusch_ch_estimator_factory, dependencies.executors.pusch_ch_estimator_executor.max_concurrency);
    report_fatal_error_if_not(pusch_ch_estimator_factory, "Invalid channel estimator factory.");
  }

  // Create DM-RS based PUSCH channel estimator factory.
  std::shared_ptr<dmrs_pusch_estimator_factory> pusch_channel_estimator_factory =
      create_dmrs_pusch_estimator_factory_sw(prg_factory,
                                             low_papr_sequence_gen_factory,
                                             pusch_ch_estimator_factory,
                                             *dependencies.executors.pusch_ch_estimator_executor.executor,
                                             pusch_chan_estimator_fd_strategy,
                                             pusch_chan_estimator_td_strategy,
                                             config.pusch_channel_estimator_compensate_cfo);
  report_error_if_not(pusch_channel_estimator_factory, "Invalid channel estimator factory.");

  // Wrap the DM-RS based PUSCH channel estimator with the metric decorator.
  if (metric_notifier) {
    pusch_channel_estimator_factory = create_pusch_channel_estimator_metric_decorator_factory(
        std::move(pusch_channel_estimator_factory), metric_notifier->get_pusch_channel_estimator_notifier());
    report_fatal_error_if_not(pusch_channel_estimator_factory, "Failed to create factory.");
  }

  // Check if a hardware-accelerated PUSCH processor is requested.
  pusch_processor_factory_sw_configuration pusch_config;
  if (!dependencies.hw_decoder_factory) {
    pusch_decoder_factory_sw_configuration decoder_config;
    decoder_config.crc_factory     = pusch_crc_calc_factory;
    decoder_config.decoder_factory = create_ldpc_decoder_factory_sw(config.ldpc_decoder_type);
    report_fatal_error_if_not(
        decoder_config.decoder_factory, "Invalid LDPC decoder factory of type {}.", config.crc_calculator_type);
    decoder_config.dematcher_factory = create_ldpc_rate_dematcher_factory_sw(config.ldpc_rate_dematcher_type);
    report_fatal_error_if_not(decoder_config.dematcher_factory,
                              "Invalid LDPC Rate Dematcher factory of type {}.",
                              config.ldpc_rate_dematcher_type);
    decoder_config.segmenter_factory = create_ldpc_segmenter_rx_factory_sw();
    report_fatal_error_if_not(decoder_config.segmenter_factory, "Invalid LDPC Rx segmenter factory.");
    decoder_config.nof_pusch_decoder_threads = dependencies.executors.pusch_decoder_executor.max_concurrency;
    decoder_config.executor                  = dependencies.executors.pusch_decoder_executor.executor;
    decoder_config.nof_prb                   = config.ul_bw_rb;
    decoder_config.nof_layers                = config.pusch_max_nof_layers;

    if (metric_notifier) {
      decoder_config.decoder_factory = create_ldpc_decoder_metric_decorator_factory(
          std::move(decoder_config.decoder_factory), metric_notifier->get_ldpc_decoder_notifier());
      report_fatal_error_if_not(decoder_config.decoder_factory, "Failed to create LDPC decoder factory.");
      decoder_config.dematcher_factory = create_ldpc_rate_dematcher_metric_decorator_factory(
          std::move(decoder_config.dematcher_factory), metric_notifier->get_ldpc_rate_dematcher_notifier());
      report_fatal_error_if_not(decoder_config.dematcher_factory, "Failed to create LDPC rate dematcher factory.");
    }

    pusch_config.decoder_factory = create_pusch_decoder_factory_sw(decoder_config);
  } else {
    pusch_decoder_factory_hw_configuration decoder_config;
    decoder_config.segmenter_factory = create_ldpc_segmenter_rx_factory_sw();
    report_fatal_error_if_not(decoder_config.segmenter_factory, "Invalid LDPC Rx segmenter factory.");
    decoder_config.crc_factory               = pusch_crc_calc_factory;
    decoder_config.hw_decoder_factory        = nullptr;
    decoder_config.nof_pusch_decoder_threads = dependencies.executors.pusch_decoder_executor.max_concurrency;
    decoder_config.hw_decoder_factory        = *dependencies.hw_decoder_factory;
    decoder_config.executor                  = dependencies.executors.pusch_decoder_executor.executor;
    pusch_config.decoder_factory             = create_pusch_decoder_factory_hw(decoder_config);
  }

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  report_fatal_error_if_not(short_block_det_factory, "Invalid short block detector factory.");

  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_dec_factory, "Invalid polar decoder factory.");

  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  report_fatal_error_if_not(uci_dec_factory, "Invalid UCI decoder factory.");

  // Enable EVM calculation if PUSCH SINR is obtained from EVM or if it is logged by the PHY.
  bool enable_evm = (config.pusch_sinr_calc_method == channel_state_information::sinr_type::evm) ||
                    (config.log_level == srslog::basic_levels::debug);

  // Enable post-equalization SINR if selected as PUSCH SINR method or if it is logged by the PHY.
  bool enable_eq_sinr = (config.pusch_sinr_calc_method == channel_state_information::sinr_type::post_equalization) ||
                        (config.log_level == srslog::basic_levels::debug);

  // Create PUSCH demodulator.
  std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
      create_pusch_demodulator_factory_sw(pusch_equalizer_factory,
                                          pusch_precoding_factory,
                                          pusch_demodulation_factory,
                                          (enable_evm) ? pusch_evm_calc_factory : nullptr,
                                          pusch_scrambling_factory,
                                          config.ul_bw_rb,
                                          enable_eq_sinr);
  report_fatal_error_if_not(uci_dec_factory, "Invalid PUSCH demodulator.");

  // Wrap PUSCH demodulator with the metric decorator if necessary.
  if (metric_notifier) {
    pusch_demod_factory = create_pusch_demodulator_metric_decorator_factory(
        std::move(pusch_demod_factory), metric_notifier->get_pusch_demodulator_notifier());
    report_fatal_error_if_not(uci_dec_factory, "Invalid PUSCH demodulator.");
  }

  // Add the remaining PUSCH processor configuration values.
  pusch_config.estimator_factory          = pusch_channel_estimator_factory;
  pusch_config.demodulator_factory        = pusch_demod_factory;
  pusch_config.demux_factory              = ulsch_demux_factory;
  pusch_config.uci_dec_factory            = uci_dec_factory;
  pusch_config.dec_nof_iterations         = config.ldpc_decoder_iterations;
  pusch_config.dec_enable_early_stop      = config.ldpc_decoder_early_stop;
  pusch_config.dec_force_decoding         = config.ldpc_decoder_force_decoding;
  pusch_config.csi_sinr_calc_method       = config.pusch_sinr_calc_method;
  pusch_config.max_nof_concurrent_threads = dependencies.executors.pusch_executor.max_concurrency;

  // :TODO: check these values in the future. Extract them to more public config.
  pusch_config.ch_estimate_dimensions.nof_symbols        = MAX_NSYMB_PER_SLOT;
  pusch_config.ch_estimate_dimensions.nof_tx_layers      = config.pusch_max_nof_layers;
  pusch_config.ch_estimate_dimensions.nof_prb            = config.ul_bw_rb;
  pusch_config.ch_estimate_dimensions.nof_rx_ports       = config.nof_rx_ports;
  std::shared_ptr<pusch_processor_factory> pusch_factory = create_pusch_processor_factory_sw(pusch_config);
  report_fatal_error_if_not(pusch_factory, "Invalid PUSCH processor factory.");

  if (metric_notifier) {
    pusch_factory = create_pusch_processor_metric_decorator_factory(std::move(pusch_factory),
                                                                    metric_notifier->get_pusch_processor_notifier());
    report_fatal_error_if_not(pusch_factory, "Failed to create factory.");
  }

  // Create synchronous PUSCH processor for UCI only.
  pusch_config.decoder_factory =
      create_pusch_decoder_empty_factory(config.ul_bw_rb, pusch_config.ch_estimate_dimensions.nof_tx_layers);
  report_fatal_error_if_not(pusch_config.decoder_factory, "Invalid PUSCH decoder factory for UCI.");
  std::shared_ptr<pusch_processor_factory> uci_proc_factory = create_pusch_processor_factory_sw(pusch_config);
  report_fatal_error_if_not(uci_proc_factory, "Invalid PUSCH processor factory for UCI.");

  // If PUSCH decoders operate in asynchronous mode the number of PUSCH regular processors is equal to the maximum
  // number of enqueued PUSCH transmissions in one slot. Otherwise, the number of PUSCH regular processors is equal to
  // the PUSCH executor maximum concurrency.
  unsigned nof_regular_processors = MAX_PUSCH_PDUS_PER_SLOT;
  if (dependencies.executors.pusch_decoder_executor.max_concurrency == 0) {
    nof_regular_processors = dependencies.executors.pusch_executor.max_concurrency;
  }

  // Create PUSCH processor pool factory.
  pusch_processor_pool_factory_config pusch_proc_pool_config;
  pusch_proc_pool_config.factory                = pusch_factory;
  pusch_proc_pool_config.uci_factory            = uci_proc_factory;
  pusch_proc_pool_config.nof_regular_processors = nof_regular_processors;
  pusch_proc_pool_config.nof_uci_processors     = dependencies.executors.pusch_executor.max_concurrency;
  pusch_factory                                 = create_pusch_processor_pool(pusch_proc_pool_config);
  report_fatal_error_if_not(pusch_factory, "Invalid PUSCH processor pool factory.");

  std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory = create_low_papr_sequence_generator_sw_factory();
  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
      create_low_papr_sequence_collection_sw_factory(lpg_factory);

  // Create channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_est_factory);
  report_fatal_error_if_not(port_chan_estimator_factory, "Invalid port channel estimator factory.");

  std::shared_ptr<dmrs_pucch_estimator_factory> pucch_dmrs_factory =
      create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, lpg_factory, port_chan_estimator_factory);
  report_fatal_error_if_not(pucch_dmrs_factory, "Invalid PUCCH DM-RS estimator factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudorandom = create_pseudo_random_generator_sw_factory();
  std::shared_ptr<pucch_detector_factory>          pucch_detector_fact =
      create_pucch_detector_factory_sw(lpc_factory, pseudorandom, pucch_equalizer_factory, dft_factory);
  report_fatal_error_if_not(pucch_detector_fact, "Invalid PUCCH detector factory.");

  // Create PUCCH demodulator factory.
  std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory = create_pucch_demodulator_factory_sw(
      pucch_equalizer_factory, demodulation_factory, prg_factory, precoding_factory);
  report_fatal_error_if_not(pucch_demod_factory, "Invalid PUCCH demodulator factory.");

  channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
  channel_estimate_dimensions.nof_tx_layers = 1;
  channel_estimate_dimensions.nof_rx_ports  = config.nof_rx_ports;
  channel_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
  channel_estimate_dimensions.nof_prb       = config.ul_bw_rb;

  // Create PUCCH processor factory.
  std::shared_ptr<pucch_processor_factory> pucch_factory = create_pucch_processor_factory_sw(
      pucch_dmrs_factory, pucch_detector_fact, pucch_demod_factory, uci_dec_factory, channel_estimate_dimensions);
  report_fatal_error_if_not(pucch_factory, "Invalid PUCCH processor factory.");

  // Create PUCCH processor pool factory if more than one thread is used.
  if (dependencies.executors.pucch_executor.max_concurrency > 1) {
    pucch_factory = create_pucch_processor_pool_factory(std::move(pucch_factory),
                                                        dependencies.executors.pucch_executor.max_concurrency);
    report_fatal_error_if_not(pucch_factory, "Invalid PUCCH processor pool factory.");
  }

  uplink_processor_impl::task_executor_collection ul_task_executors = {
      .pucch_executor = *dependencies.executors.pucch_executor.executor,
      .pusch_executor = *dependencies.executors.pusch_executor.executor,
      .srs_executor   = *dependencies.executors.srs_executor.executor,
      .prach_executor = *dependencies.executors.prach_executor.executor};

  // Create base factory.
  std::shared_ptr<uplink_processor_factory> factory =
      std::make_shared<uplink_processor_base_factory>(std::move(pucch_factory),
                                                      std::move(pusch_factory),
                                                      std::move(prach_factory),
                                                      std::move(srs_factory),
                                                      rg_factory,
                                                      ul_task_executors);
  report_fatal_error_if_not(factory, "Invalid Uplink processor factory.");

  return factory;
}

static std::unique_ptr<uplink_processor_pool>
create_ul_processor_pool(uplink_processor_factory&              factory,
                         rx_buffer_pool&                        rm_buffer_pool,
                         upper_phy_rx_results_notifier&         rx_results_notifier,
                         const upper_phy_factory_configuration& factory_config,
                         const upper_phy_configuration&         config)
{
  uplink_processor_pool_config config_pool;

  // Fetch and configure logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY", true);
  logger.set_level(factory_config.log_level);
  logger.set_hex_dump_max_size(factory_config.logger_max_hex_size);

  for (unsigned scs = 0, scs_end = static_cast<unsigned>(subcarrier_spacing::invalid); scs != scs_end; ++scs) {
    // Skip SCS if not active.
    if (!config.active_scs[scs]) {
      continue;
    }

    uplink_processor_pool_config::uplink_processor_set info;
    info.scs = to_subcarrier_spacing(scs);

    // Prepare UL processor configuration.
    uplink_processor_config ul_proc_config = {.notifier       = rx_results_notifier,
                                              .rm_buffer_pool = rm_buffer_pool,
                                              .nof_rx_ports   = config.nof_rx_ports,
                                              .nof_rb         = config.ul_bw_rb,
                                              .max_nof_layers = config.pusch_max_nof_layers};

    for (unsigned count = 0; count != config.nof_ul_rg; ++count) {
      // Create an uplink processor.
      std::unique_ptr<uplink_processor> ul_proc;
      if (factory_config.log_level != srslog::basic_levels::none) {
        ul_proc = factory.create(ul_proc_config, logger, factory_config.enable_logging_broadcast);
      } else {
        ul_proc = factory.create(ul_proc_config);
      }
      report_fatal_error_if_not(ul_proc, "Invalid uplink processor.");

      info.procs.push_back(std::move(ul_proc));
    }

    config_pool.ul_processors.push_back(std::move(info));
    if (factory_config.log_level != srslog::basic_levels::none) {
      config_pool.default_processor = factory.create(ul_proc_config, logger, factory_config.enable_logging_broadcast);
    } else {
      config_pool.default_processor = factory.create(ul_proc_config);
    }
  }

  return create_uplink_processor_pool(std::move(config_pool));
}

static std::unique_ptr<prach_buffer_pool> create_prach_pool(const upper_phy_configuration& config)
{
  std::vector<std::unique_ptr<prach_buffer>> prach_mem;
  prach_mem.reserve(config.nof_prach_buffer);

  for (unsigned i = 0, e = config.nof_prach_buffer; i != e; ++i) {
    std::unique_ptr<prach_buffer> buffer;

    if (config.is_prach_long_format) {
      buffer = create_prach_buffer_long(config.nof_rx_ports, config.max_nof_fd_prach_occasions);
    } else {
      buffer = create_prach_buffer_short(
          config.nof_rx_ports, config.max_nof_td_prach_occasions, config.max_nof_fd_prach_occasions);
    }

    report_fatal_error_if_not(buffer, "Invalid PRACH buffer.");
    prach_mem.push_back(std::move(buffer));
  }

  return create_prach_buffer_pool(std::move(prach_mem));
}

std::shared_ptr<upper_phy_rx_symbol_handler_factory> srsran::create_rx_symbol_handler_factory()
{
  return std::make_shared<upper_phy_rx_symbol_handler_factory_impl>();
}

#ifndef SRSRAN_HAS_PHY_TAP
std::shared_ptr<upper_phy_rx_symbol_handler_factory>
srsran::create_rx_symbol_handler_tap_factory(std::shared_ptr<upper_phy_rx_symbol_handler_factory> factory,
                                             unsigned                                             nof_rb,
                                             unsigned                                             nof_ports,
                                             const std::string&                                   processor_arguments)
{
  // Return the factory as is, since the PHY tap plugin is not present.
  return factory;
}
#endif

std::shared_ptr<upper_phy_rx_symbol_handler_factory> srsran::create_rx_symbol_handler_printer_decorator_factory(
    std::shared_ptr<upper_phy_rx_symbol_handler_factory> factory_,
    srslog::basic_logger&                                logger_,
    const std::string&                                   filename,
    unsigned                                             nof_rb,
    interval<unsigned>                                   ul_print_ports,
    bool                                                 print_prach_)
{
  return std::make_shared<upper_phy_rx_symbol_handler_printer_decorator_factory>(
      std::move(factory_), logger_, filename, nof_rb, ul_print_ports, print_prach_);
}

std::shared_ptr<downlink_processor_factory>
srsran::create_downlink_processor_factory_sw(const downlink_processor_factory_sw_config&       config,
                                             const downlink_processor_factory_sw_dependencies& dependencies)
{
  // Create channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  report_fatal_error_if_not(precoding_factory, "Invalid channel precoder factory.");

  // Create resource grid mapper factory.
  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
      create_resource_grid_mapper_factory(precoding_factory);
  report_fatal_error_if_not(precoding_factory, "Invalid resource grid mapper factory.");

  // Create channel coding factories - CRC
  std::shared_ptr<crc_calculator_factory> crc_calc_factory =
      create_crc_calculator_factory_sw(config.crc_calculator_type);
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC calculator factory of type {}.", config.crc_calculator_type);

  std::shared_ptr<ldpc_encoder_factory> ldpc_enc_factory = create_ldpc_encoder_factory_sw(config.ldpc_encoder_type);
  report_fatal_error_if_not(ldpc_enc_factory, "Invalid LDPC encoder factory of type {}.", config.ldpc_encoder_type);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rm_factory = create_ldpc_rate_matcher_factory_sw();
  report_fatal_error_if_not(ldpc_rm_factory, "Invalid LDPC RM factory.");

  // Create channel coding factories - Polar
  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_factory, "Invalid POLAR factory.");

  // Create sequence generators factories - PRG
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");

  // Create modulation mapper factory.
  std::shared_ptr<modulation_mapper_factory> mod_factory = create_modulation_mapper_factory();
  report_fatal_error_if_not(mod_factory, "Invalid modulation factory.");

  // Create metric decorators if applicable.
  std::shared_ptr<pseudo_random_generator_factory> pdsch_scrambling_factory = prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    pdsch_rg_mapper_factory  = rg_mapper_factory;
  std::shared_ptr<crc_calculator_factory>          pdsch_crc_calc_factory   = crc_calc_factory;
  std::shared_ptr<modulation_mapper_factory>       pdsch_mod_mapper_factory = mod_factory;
  if (dependencies.metric_notifier) {
    std::shared_ptr<channel_precoder_factory> pdsch_precoding_factory = precoding_factory;
    pdsch_precoding_factory = create_channel_precoder_metric_decorator_factory(
        std::move(pdsch_precoding_factory), dependencies.metric_notifier->get_pdsch_channel_precoder_notifier());
    report_fatal_error_if_not(pdsch_precoding_factory, "Invalid PDSCH precoding factory.");

    pdsch_rg_mapper_factory = create_resource_grid_mapper_factory(pdsch_precoding_factory);
    report_fatal_error_if_not(pdsch_rg_mapper_factory, "Invalid resource grid mapper factory.");

    ldpc_enc_factory = create_ldpc_encoder_metric_decorator_factory(
        std::move(ldpc_enc_factory), dependencies.metric_notifier->get_ldpc_encoder_notifier());
    report_fatal_error_if_not(ldpc_enc_factory, "Invalid LDPC encoder factory.");

    ldpc_rm_factory = create_ldpc_rate_matcher_metric_decorator_factory(
        std::move(ldpc_rm_factory), dependencies.metric_notifier->get_ldpc_rate_matcher_notifier());
    report_fatal_error_if_not(ldpc_enc_factory, "Invalid LDPC rate matcher factory.");

    pdsch_scrambling_factory = create_pseudo_random_generator_metric_decorator_factory(
        std::move(pdsch_scrambling_factory), dependencies.metric_notifier->get_pdsch_scrambling_notifier());
    report_fatal_error_if_not(ldpc_enc_factory, "Invalid PDSCH scrambling factory.");

    pdsch_crc_calc_factory = create_crc_calculator_metric_decorator_factory(
        std::move(pdsch_crc_calc_factory), dependencies.metric_notifier->get_pdsch_crc_calculator_notifier());
    report_fatal_error_if_not(pdsch_crc_calc_factory, "Invalid PDSCH CRC calculator factory.");

    pdsch_mod_mapper_factory = create_modulation_mapper_metric_decorator_factory(
        std::move(pdsch_mod_mapper_factory), dependencies.metric_notifier->get_pdsch_modulation_mapper_notifier());
    report_fatal_error_if_not(pdsch_mod_mapper_factory, "Invalid PDSCH modulation mapper factory.");
  }

  // Create channel coding factories - LDPC
  std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_seg_tx_factory =
      create_ldpc_segmenter_tx_factory_sw(pdsch_crc_calc_factory);
  report_fatal_error_if_not(ldpc_seg_tx_factory, "Invalid LDPC segmenter factory.");

  // Create channel processors encoder factories - PBCH
  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  report_fatal_error_if_not(pbch_enc_factory, "Invalid PBCH encoder factory.");

  // Create channel processors encoder factories - PDCCH
  std::shared_ptr<pdcch_encoder_factory> pdcch_enc_factory =
      create_pdcch_encoder_factory_sw(crc_calc_factory, polar_factory);
  report_fatal_error_if_not(pdcch_enc_factory, "Invalid PDCCH encoder factory.");

  // Create channel processors encoder factories - PDSCH. Check if hardware acceleration is requested.
  std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = nullptr;
  if (!config.hw_encoder_factory) {
    // Create a software PDSCH encoder factory.
    pdsch_encoder_factory_sw_configuration pdsch_enc_factory_config;
    pdsch_enc_factory_config.segmenter_factory    = ldpc_seg_tx_factory;
    pdsch_enc_factory_config.encoder_factory      = ldpc_enc_factory;
    pdsch_enc_factory_config.rate_matcher_factory = ldpc_rm_factory;
    pdsch_enc_factory                             = create_pdsch_encoder_factory_sw(pdsch_enc_factory_config);
  } else {
    // Create a hardware-accelerated PDSCH encoder factory.
    pdsch_encoder_factory_hw_configuration encoder_config;
    encoder_config.crc_factory        = pdsch_crc_calc_factory;
    encoder_config.segmenter_factory  = ldpc_seg_tx_factory;
    encoder_config.hw_encoder_factory = *config.hw_encoder_factory;
    pdsch_enc_factory                 = create_pdsch_encoder_factory_hw(encoder_config);
  }
  report_fatal_error_if_not(pdsch_enc_factory, "Invalid PDSCH encoder factory.");

  // Create channel processors modulation factories - PBCH
  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory = create_pbch_modulator_factory_sw(mod_factory, prg_factory);
  report_fatal_error_if_not(pbch_mod_factory, "Invalid PBCH modulation factory.");

  // Create channel processors modulation factories - PDCCH
  std::shared_ptr<pdcch_modulator_factory> pdcch_mod_factory =
      create_pdcch_modulator_factory_sw(mod_factory, prg_factory, rg_mapper_factory);
  report_fatal_error_if_not(pdcch_mod_factory, "Invalid PDCCH modulation factory.");

  // Create channel processors modulation factories - PDSCH
  std::shared_ptr<pdsch_modulator_factory> pdsch_mod_factory =
      create_pdsch_modulator_factory_sw(pdsch_mod_mapper_factory, pdsch_scrambling_factory, pdsch_rg_mapper_factory);
  report_fatal_error_if_not(pdsch_mod_factory, "Invalid PDSCH modulation factory.");

  // Create DMRS generators - PBCH, PSS, SSS
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_pbch_proc_factory =
      create_dmrs_pbch_processor_factory_sw(prg_factory);
  report_fatal_error_if_not(dmrs_pbch_proc_factory, "Invalid DMRS PBCH factory.");

  std::shared_ptr<pss_processor_factory> pss_proc_factory = create_pss_processor_factory_sw();
  report_fatal_error_if_not(pss_proc_factory, "Invalid PSS factory.");

  std::shared_ptr<sss_processor_factory> sss_proc_factory = create_sss_processor_factory_sw();
  report_fatal_error_if_not(sss_proc_factory, "Invalid SSS factory.");

  // Create PDCCH DM-RS generators.
  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_proc_factory =
      create_dmrs_pdcch_processor_factory_sw(prg_factory, rg_mapper_factory);
  report_fatal_error_if_not(dmrs_pdcch_proc_factory, "Invalid DMRS PDCCH factory.");

  // Create PDSCH DM-RS generator.
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory, rg_mapper_factory);
  report_fatal_error_if_not(dmrs_pdsch_proc_factory, "Invalid DMRS PDSCH factory.");

  // Wrap DM-RS for PDSCH factory with a metric decorator.
  if (dependencies.metric_notifier) {
    dmrs_pdsch_proc_factory = create_dmrs_pdsch_generator_metric_decorator_factory(
        std::move(dmrs_pdsch_proc_factory), dependencies.metric_notifier->get_pdsch_dmrs_generator_notifier());
    report_fatal_error_if_not(dmrs_pdsch_proc_factory, "Invalid metric DM-RS PDSCH generator factory.");
  }

  // Create PTRS generators - PDSCH
  std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_pdsch_gen_factory =
      create_ptrs_pdsch_generator_generic_factory(prg_factory, rg_mapper_factory);
  report_fatal_error_if_not(ptrs_pdsch_gen_factory, "Invalid PTRS PDSCH factory.");

  // Create channel processors - PDCCH
  std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory =
      create_pdcch_processor_factory_sw(pdcch_enc_factory, pdcch_mod_factory, dmrs_pdcch_proc_factory);
  report_fatal_error_if_not(pdcch_proc_factory, "Invalid PDCCH processor factory.");

  // Create PDSCH block processor factory (if needed).
  std::shared_ptr<pdsch_block_processor_factory> block_processor_factory;
  if (!std::holds_alternative<pdsch_processor_generic_configuration>(config.pdsch_processor)) {
    if (!config.hw_encoder_factory.has_value()) {
      block_processor_factory = create_pdsch_block_processor_factory_sw(
          ldpc_enc_factory, ldpc_rm_factory, pdsch_scrambling_factory, pdsch_mod_mapper_factory);
      report_fatal_error_if_not(block_processor_factory, "Invalid SW PDSCH block processor factory.");
    } else {
      block_processor_factory = create_pdsch_block_processor_factory_hw(
          *config.hw_encoder_factory, pdsch_scrambling_factory, pdsch_mod_mapper_factory);
      report_fatal_error_if_not(block_processor_factory, "Invalid HW PDSCH block processor factory.");
    }
  }

  // Create channel processors - PDSCH
  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory;
  unsigned                                 pdsch_processor_pool_size = config.pdsch_executor.max_concurrency;
  if (std::holds_alternative<pdsch_processor_generic_configuration>(config.pdsch_processor) ||
      !dependencies.pdsch_codeblock_executor.is_valid()) {
    pdsch_proc_factory = create_pdsch_processor_factory_sw(
        pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_proc_factory, ptrs_pdsch_gen_factory);
    report_fatal_error_if_not(pdsch_proc_factory, "Invalid generic PDSCH processor factory.");
  } else if (std::holds_alternative<pdsch_processor_flexible_configuration>(config.pdsch_processor)) {
    const pdsch_processor_flexible_configuration& pdsch_processor_config =
        std::get<pdsch_processor_flexible_configuration>(config.pdsch_processor);
    report_fatal_error_if_not(dependencies.pdsch_codeblock_executor.max_concurrency >= 1,
                              "The number of threads (i.e., {}) must be equal to or greater than 1.");

    // Create a flexible PDSCH processor factory.
    pdsch_proc_factory =
        create_pdsch_flexible_processor_factory_sw(ldpc_seg_tx_factory,
                                                   block_processor_factory,
                                                   pdsch_rg_mapper_factory,
                                                   dmrs_pdsch_proc_factory,
                                                   ptrs_pdsch_gen_factory,
                                                   *dependencies.pdsch_codeblock_executor.executor,
                                                   dependencies.pdsch_codeblock_executor.max_concurrency,
                                                   pdsch_processor_config.cb_batch_length);

    // The number of required processors in the pool must be equal to the maximum number of PDSCH that can be enqueued
    // in a single slot if the PDSCH processor operates asynchronously.
    if (pdsch_processor_config.cb_batch_length != pdsch_processor_flexible_configuration::synchronous_cb_batch_length) {
      pdsch_processor_pool_size = MAX_PDSCH_PDUS_PER_SLOT;
    }
    report_fatal_error_if_not(pdsch_proc_factory, "Invalid flexible PDSCH processor factory.");
  }

  // Create a PDSCH processor metric decorator factory on the top of the PDSCH processor factory.
  if (dependencies.metric_notifier) {
    pdsch_proc_factory = create_pdsch_processor_metric_decorator_factory(
        std::move(pdsch_proc_factory), dependencies.metric_notifier->get_pdsch_processor_notifier());
    report_fatal_error_if_not(pdsch_proc_factory, "Invalid PDSCH processor metric decorator factory.");
  }

  // Create channel processors - SSB
  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                      = pbch_enc_factory;
  ssb_factory_config.modulator_factory                    = pbch_mod_factory;
  ssb_factory_config.dmrs_factory                         = dmrs_pbch_proc_factory;
  ssb_factory_config.pss_factory                          = pss_proc_factory;
  ssb_factory_config.sss_factory                          = sss_proc_factory;
  std::shared_ptr<ssb_processor_factory> ssb_proc_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  report_fatal_error_if_not(ssb_proc_factory, "Invalid SSB processor factory.");

  // Create signal generators - NZP-CSI-RS
  std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory =
      create_nzp_csi_rs_generator_factory_sw(prg_factory, rg_mapper_factory);
  report_fatal_error_if_not(nzp_csi_rs_factory, "Invalid NZP-CSI-RS generator factory.");

  // Create signal generators - PRS
  std::shared_ptr<prs_generator_factory> prs_gen_factory =
      create_prs_generator_generic_factory(prg_factory, precoding_factory);
  report_fatal_error_if_not(prs_gen_factory, "Invalid PRS generator factory.");

  // Wrap PDSCH processor factory with a concurrent pool.
  if (pdsch_processor_pool_size > 1) {
    pdsch_proc_factory = create_pdsch_processor_pool(std::move(pdsch_proc_factory), pdsch_processor_pool_size);
    report_fatal_error_if_not(pdsch_proc_factory, "Invalid asynchronous PDSCH processor pool factory.");
  }

  // Wrap PDCCH processor factory with a concurrent pool.
  if (config.pdcch_executor.max_concurrency > 1) {
    pdcch_proc_factory =
        create_pdcch_processor_pool_factory(std::move(pdcch_proc_factory), config.pdcch_executor.max_concurrency);
    report_fatal_error_if_not(pdcch_proc_factory, "Invalid PDCCH processor pool factory.");
  }

  // Wrap SSB processor factory with a concurrent pool.
  if (config.ssb_executor.max_concurrency > 1) {
    ssb_proc_factory =
        create_ssb_processor_pool_factory(std::move(ssb_proc_factory), config.ssb_executor.max_concurrency);
    report_fatal_error_if_not(ssb_proc_factory, "Invalid SSB processor pool factory.");
  }

  // Wrap NZP-CSI-RS generator factory with a concurrent pool.
  if (config.csi_rs_executor.max_concurrency > 1) {
    nzp_csi_rs_factory =
        create_nzp_csi_rs_generator_pool_factory(std::move(nzp_csi_rs_factory), config.csi_rs_executor.max_concurrency);
    report_fatal_error_if_not(nzp_csi_rs_factory, "Invalid NZP-CSI-RS generator pool factory.");
  }

  // Wrap PRS generator factory with a concurrent pool.
  if (config.prs_executor.max_concurrency > 1) {
    prs_gen_factory =
        create_prs_generator_pool_factory(std::move(prs_gen_factory), config.prs_executor.max_concurrency);
    report_fatal_error_if_not(prs_gen_factory, "Invalid PRS generator pool factory.");
  }

  downlink_processor_single_executor_factory::factory_configuration dl_proc_config = {
      .pdcch_proc_factory = pdcch_proc_factory,
      .pdsch_proc_factory = pdsch_proc_factory,
      .ssb_proc_factory   = ssb_proc_factory,
      .nzp_csi_rs_factory = nzp_csi_rs_factory,
      .prs_gen_factory    = prs_gen_factory,
      .pdcch_executor     = config.pdcch_executor.executor,
      .pdsch_executor     = config.pdsch_executor.executor,
      .ssb_executor       = config.ssb_executor.executor,
      .csi_rs_executor    = config.csi_rs_executor.executor,
      .prs_executor       = config.prs_executor.executor,
  };

  return std::make_shared<downlink_processor_single_executor_factory>(dl_proc_config);
}

std::unique_ptr<uplink_processor_pool> srsran::create_uplink_processor_pool(uplink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  uplink_processor_pool_impl_config ul_processors;

  for (auto& proc : config.ul_processors) {
    ul_processors.procs.push_back({proc.scs, std::move(proc.procs)});
  }
  ul_processors.default_processor = std::move(config.default_processor);

  return std::make_unique<uplink_processor_pool_impl>(std::move(ul_processors));
}

std::unique_ptr<downlink_processor_pool> srsran::create_dl_processor_pool(downlink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  downlink_processor_pool_impl_config dl_processors;

  for (auto& proc : config.dl_processors) {
    dl_processors.procs.push_back({proc.scs, std::move(proc.procs)});
  }

  return std::make_unique<downlink_processor_pool_impl>(std::move(dl_processors));
}

std::unique_ptr<upper_phy_factory>
srsran::create_upper_phy_factory(const upper_phy_factory_configuration& factory_config,
                                 const upper_phy_factory_dependencies&  factory_deps)
{
  return std::make_unique<upper_phy_factory_impl>(factory_config, factory_deps);
}
