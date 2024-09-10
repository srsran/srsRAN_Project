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
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/units/flexible_du/du_high/du_high_commands.h"
#include "apps/units/flexible_du/du_high/du_high_config_translators.h"
#include "apps/units/flexible_du/du_high/du_high_wrapper_config_helper.h"
#include "apps/units/flexible_du/du_low/du_low_config_translator.h"
#include "apps/units/flexible_du/du_low/du_low_wrapper_config_helper.h"
#include "apps/units/flexible_du/flexible_du_commands.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_factories.h"
#include "apps/units/flexible_du/split_8/ru_sdr_factories.h"
#include "dynamic_du_impl.h"
#include "dynamic_du_translators.h"
#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_factory.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ru/ru_dummy_factory.h"
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#endif // DPDK_FOUND

using namespace srsran;

static std::unique_ptr<radio_unit> create_dummy_radio_unit(const ru_dummy_unit_config& ru_cfg,
                                                           unsigned                    max_processing_delay_slots,
                                                           unsigned                    nof_prach_ports,
                                                           worker_manager&             workers,
                                                           span<const srs_du::du_cell_config>  du_cells,
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
create_radio_unit(const std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
                  worker_manager&                                                                          workers,
                  span<const srs_du::du_cell_config>                                                       du_cells,
                  ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                  ru_timing_notifier&                 timing_notifier,
                  ru_error_notifier&                  error_notifier,
                  unsigned                            max_processing_delay,
                  unsigned                            prach_nof_ports)
{
  if (std::holds_alternative<ru_ofh_unit_parsed_config>(ru_cfg)) {
    ru_ofh_factory_config config;
    config.ru_cfg                     = std::get<ru_ofh_unit_parsed_config>(ru_cfg).config;
    config.max_processing_delay_slots = max_processing_delay;
    config.du_cells                   = du_cells;

    ru_ofh_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_ofh_radio_unit(config, dependencies);
  }

  if (std::holds_alternative<ru_sdr_unit_config>(ru_cfg)) {
    ru_sdr_factory_config config;
    config.du_cells                   = du_cells;
    config.ru_cfg                     = std::get<ru_sdr_unit_config>(ru_cfg);
    config.max_processing_delay_slots = max_processing_delay;

    ru_sdr_factory_dependencies dependencies;
    dependencies.workers         = &workers;
    dependencies.error_notifier  = &error_notifier;
    dependencies.symbol_notifier = &symbol_notifier;
    dependencies.timing_notifier = &timing_notifier;

    return create_sdr_radio_unit(config, dependencies);
  }

  return create_dummy_radio_unit(std::get<ru_dummy_unit_config>(ru_cfg),
                                 max_processing_delay,
                                 prach_nof_ports,
                                 workers,
                                 du_cells,
                                 symbol_notifier,
                                 timing_notifier,
                                 error_notifier);
}

/// \brief Update the Flexible DU metrics configuration with the given local DU configuration and E2 configuration.
///
/// This function manages the multicell workaround for the DU high metrics. To have multicell, now one DU is
/// instantiated per cell, so the this would create multiple consumers that does not make sense, for example stdout.
/// With this we avoid having 2 different objects that write in the stdout.
static void update_du_metrics(std::vector<app_services::metrics_config>& flexible_du_cfg,
                              std::vector<app_services::metrics_config>  local_du_cfg,
                              bool                                       is_e2_enabled)
{
  // First call, copy everything.
  if (flexible_du_cfg.empty()) {
    flexible_du_cfg = std::move(local_du_cfg);
    return;
  }

  // Safe check that all the DUs provides the same amount of metrics.
  srsran_assert(flexible_du_cfg.size() == local_du_cfg.size(),
                "Flexible DU metrics size '{}' does not match DU metrics size '{}'",
                flexible_du_cfg.size(),
                local_du_cfg.size());

  // Iterate the metrics configs of each DU. Each DU should ha
  for (unsigned i = 0, e = local_du_cfg.size(); i != e; ++i) {
    // Store the metrics producers for each DU.
    flexible_du_cfg[i].producers.push_back(std::move(local_du_cfg[i].producers.back()));

    // Move E2 consumers for each DU to the common output config. E2 Consumers occupy the last position.
    if (is_e2_enabled) {
      flexible_du_cfg[i].consumers.push_back(std::move(local_du_cfg[i].consumers.back()));
    }
  }
}

du_unit srsran::create_du(const dynamic_du_unit_config& dyn_du_cfg, du_unit_dependencies& dependencies)
{
  du_unit du_cmd_wrapper;

  const du_high_unit_config& du_hi    = dyn_du_cfg.du_high_cfg.config;
  const du_low_unit_config&  du_lo    = dyn_du_cfg.du_low_cfg;
  const fapi_unit_config&    fapi_cfg = dyn_du_cfg.fapi_cfg;

  auto du_cells = generate_du_cell_config(du_hi);

  std::vector<std::unique_ptr<srs_du::du_wrapper>> du_insts;
  auto                                             du_impl = std::make_unique<dynamic_du_impl>(du_cells.size());

  std::vector<srs_du::cell_prach_ports_entry> prach_ports;
  std::vector<unsigned>                       max_pusch_per_slot;
  for (const auto& high : du_hi.cells_cfg) {
    prach_ports.push_back(high.cell.prach_cfg.ports);
    max_pusch_per_slot.push_back(high.cell.pusch_cfg.max_puschs_per_slot);
  }

  // Initialize hardware-accelerator (only if needed).
  hal_upper_phy_config hal_config  = {};
  hal_config.hwacc_pdsch_processor = false;
  hal_config.hwacc_pusch_processor = false;
#ifdef DPDK_FOUND
  hal::bbdev_hwacc_pdsch_enc_factory_configuration         hwacc_pdsch_enc_cfg = {};
  hal::bbdev_hwacc_pusch_dec_factory_configuration         hwacc_pusch_dec_cfg = {};
  std::shared_ptr<hal::ext_harq_buffer_context_repository> harq_buffer_context = nullptr;
  unsigned                                                 nof_hwacc_dus       = du_cells.size();
  if (!du_lo.hal_config->bbdev_hwacc->hwacc_type.empty()) {
    srslog::basic_logger& hwacc_logger = srslog::fetch_basic_logger("HWACC", false);
    hwacc_logger.set_level(du_lo.loggers.hal_level);

    // Create a bbdev accelerator factory.
    std::unique_ptr<dpdk::bbdev_acc_factory> bbdev_acc_factory =
        srsran::dpdk::create_bbdev_acc_factory(du_lo.hal_config->bbdev_hwacc->bbdev_acc_type);
    report_error_if_not(bbdev_acc_factory,
                        "Unable to create the {} bbdev hardware-accelerator interface factory.",
                        du_lo.hal_config->bbdev_hwacc->bbdev_acc_type);

    // Intefacing to the bbdev-based hardware-accelerator.
    dpdk::bbdev_acc_configuration bbdev_config;
    bbdev_config.id = du_lo.hal_config->bbdev_hwacc->id;
    if (du_lo.hal_config->bbdev_hwacc->pdsch_enc->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_enc_lcores = nof_hwacc_dus * du_lo.hal_config->bbdev_hwacc->pdsch_enc->nof_hwacc;
    }
    if (du_lo.hal_config->bbdev_hwacc->pusch_dec->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_dec_lcores = nof_hwacc_dus * du_lo.hal_config->bbdev_hwacc->pusch_dec->nof_hwacc;
    }
    // If no msg_mbuf size is defined, a worst-case value will be used.
    bbdev_config.msg_mbuf_size = du_lo.hal_config->bbdev_hwacc->msg_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no rm_mbuf size is defined, a worst-case value will be used.
    bbdev_config.rm_mbuf_size = du_lo.hal_config->bbdev_hwacc->rm_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no number of mbufs is defined, a worst-case value will be used.
    bbdev_config.nof_mbuf =
        du_lo.hal_config->bbdev_hwacc->nof_mbuf.value_or(static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS))));
    std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = bbdev_acc_factory->create(bbdev_config, hwacc_logger);
    report_error_if_not(
        bbdev_accelerator, "Unable to open the {} hardware-accelerator.", du_lo.hal_config->bbdev_hwacc->hwacc_type);

    // Configure the hardware-accelerated PDSCH encoding factory (only if needed).
    if (du_lo.hal_config->bbdev_hwacc->pdsch_enc->nof_hwacc > 0) {
      hwacc_pdsch_enc_cfg.acc_type          = du_lo.hal_config->bbdev_hwacc->hwacc_type;
      hwacc_pdsch_enc_cfg.bbdev_accelerator = bbdev_accelerator;
      hwacc_pdsch_enc_cfg.cb_mode           = du_lo.hal_config->bbdev_hwacc->pdsch_enc->cb_mode;
      // If no maximum buffer size is defined, a worst-case value will be used.
      hwacc_pdsch_enc_cfg.max_tb_size =
          du_lo.hal_config->bbdev_hwacc->pdsch_enc->max_buffer_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
      hwacc_pdsch_enc_cfg.dedicated_queue = du_lo.hal_config->bbdev_hwacc->pdsch_enc->dedicated_queue;
      hal_config.hwacc_pdsch_processor    = true;
      hal_config.hwacc_pdsch_enc_cfg      = hwacc_pdsch_enc_cfg;
    }

    // Configure the hardware-accelerated PUSCH decoding factory (only if needed).
    if (du_lo.hal_config->bbdev_hwacc->pusch_dec->nof_hwacc > 0) {
      hwacc_pusch_dec_cfg.acc_type          = du_lo.hal_config->bbdev_hwacc->hwacc_type;
      hwacc_pusch_dec_cfg.bbdev_accelerator = bbdev_accelerator;
      hwacc_pusch_dec_cfg.ext_softbuffer    = du_lo.hal_config->bbdev_hwacc->pusch_dec->ext_softbuffer;
      if (hwacc_pusch_dec_cfg.ext_softbuffer) {
        // Set up an external HARQ buffer context repository.
        unsigned nof_cbs = du_lo.hal_config->bbdev_hwacc->pusch_dec->harq_context_size.value_or(MAX_NOF_SEGMENTS);
        uint64_t ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
        harq_buffer_context = hal::create_ext_harq_buffer_context_repository(nof_cbs, ext_harq_buff_size, false);
        report_error_if_not(harq_buffer_context,
                            "Unable to create the external HARQ buffer context for the {} hardware-accelerator.",
                            du_lo.hal_config->bbdev_hwacc->hwacc_type);
        hwacc_pusch_dec_cfg.harq_buffer_context = harq_buffer_context;
      }
      hwacc_pusch_dec_cfg.dedicated_queue = du_lo.hal_config->bbdev_hwacc->pusch_dec->dedicated_queue;
      hal_config.hwacc_pusch_processor    = true;
      hal_config.hwacc_pusch_dec_cfg      = hwacc_pusch_dec_cfg;
    }
  }
#endif // DPDK_FOUND

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    // Create one DU per cell.
    srs_du::du_wrapper_config du_cfg  = {};
    du_high_unit_config       tmp_cfg = du_hi;
    tmp_cfg.cells_cfg.resize(1);
    tmp_cfg.cells_cfg[0] = du_hi.cells_cfg[i];

    make_du_low_wrapper_config_and_dependencies(du_cfg.du_low_cfg,
                                                du_lo,
                                                hal_config,
                                                {prach_ports[i]},
                                                span<const srs_du::du_cell_config>(&du_cells[i], 1),
                                                span<const unsigned>(&max_pusch_per_slot[i], 1),
                                                du_impl->get_upper_ru_dl_rg_adapter(),
                                                du_impl->get_upper_ru_ul_request_adapter(),
                                                *dependencies.workers,
                                                i);

    auto cell_services_cfg = fill_du_high_wrapper_config(du_cfg.du_high_cfg,
                                                         tmp_cfg,
                                                         i,
                                                         dependencies.workers->get_du_high_executor_mapper(i),
                                                         *dependencies.f1c_client_handler,
                                                         *dependencies.f1u_gw,
                                                         *dependencies.timer_mng,
                                                         *dependencies.mac_p,
                                                         *dependencies.rlc_p,
                                                         *dependencies.e2_client_handler,
                                                         *dependencies.e2_metric_connectors,
                                                         *dependencies.json_sink,
                                                         *dependencies.metrics_notifier);

    update_du_metrics(du_cmd_wrapper.metrics, std::move(cell_services_cfg.first), tmp_cfg.e2_cfg.enable_du_e2);

    // Use the commands of the first cell.
    if (i == 0) {
      for (auto& command : cell_services_cfg.second)
        du_cmd_wrapper.commands.push_back(std::move(command));
    }

    // FAPI configuration.
    du_cfg.du_high_cfg.fapi.log_level = fapi_cfg.fapi_level;
    if (fapi_cfg.l2_nof_slots_ahead != 0) {
      // As the temporal configuration contains only one cell, pick the data from that cell.
      du_cfg.du_high_cfg.fapi.l2_nof_slots_ahead = fapi_cfg.l2_nof_slots_ahead;
      du_cfg.du_high_cfg.fapi.executor.emplace(dependencies.workers->fapi_exec[i]);
    } else {
      report_error_if_not(dependencies.workers->fapi_exec[i] == nullptr,
                          "FAPI buffered worker created for a cell with no MAC delay configured");
    }

    du_insts.push_back(make_du_wrapper(du_cfg));
    report_error_if_not(du_insts.back(), "Invalid Distributed Unit");
  }

  std::unique_ptr<radio_unit> ru = create_radio_unit(dyn_du_cfg.ru_cfg,
                                                     *dependencies.workers,
                                                     du_cells,
                                                     du_impl->get_upper_ru_ul_adapter(),
                                                     du_impl->get_upper_ru_timing_adapter(),
                                                     du_impl->get_upper_ru_error_adapter(),
                                                     du_lo.expert_phy_cfg.max_processing_delay_slots,
                                                     du_hi.cells_cfg.front().cell.prach_cfg.ports.size());

  srsran_assert(ru, "Invalid Radio Unit");

  // Add RU commands.
  du_cmd_wrapper.commands.push_back(std::make_unique<change_log_level_app_command>());
  du_cmd_wrapper.commands.push_back(std::make_unique<ru_metrics_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<tx_gain_app_command>(ru->get_controller()));
  du_cmd_wrapper.commands.push_back(std::make_unique<rx_gain_app_command>(ru->get_controller()));

  du_impl->add_ru(std::move(ru));
  du_impl->add_dus(std::move(du_insts));

  du_cmd_wrapper.unit = std::move(du_impl);

  // Configure the application unit metrics for the DU high.
  announce_du_high_cells(du_hi);

  return du_cmd_wrapper;
}
