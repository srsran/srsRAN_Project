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

#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/signal_handler.h"
#include "srsran/support/tsan_options.h"
#include "srsran/support/version/version.h"

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"

#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"

#include "adapters/ngap_adapter.h"
#include "srsran/support/io_broker/io_broker_factory.h"

#include "adapters/e1ap_adapter.h"
#include "adapters/f1ap_adapter.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/executors/task_worker_pool.h"

#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"

#include "helpers/gnb_console_helper.h"

#include "fapi_factory.h"
#include "lib/du_high/du_high.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "lib/pcap/dlt_pcap_impl.h"
#include "lib/pcap/mac_pcap_impl.h"
#include "phy_factory.h"
#include "srsran/fapi/logging_decorator_factories.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"
#include "srsran/ru/ru_adapters.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_generic_factory.h"
#include "srsran/ru/ru_ofh_factory.h"

#include "srsran/support/executors/priority_multiqueue_task_worker.h"
#include "srsran/support/sysinfo.h"
#include <atomic>
#include <unordered_map>

using namespace srsran;

/// \file
/// \brief Application of a co-located gNB with combined distributed unit (DU) and centralized unit (CU).
///
/// This application runs a gNB without the the F1 connection between CU and DU and without the E1 connection
/// between the CU-CP and CU-UP going over a real SCTP connection. However, its does expose the N2 and N3 interface
/// to the AMF and UPF over the standard SCTP ports.
/// The app serves as an example for a all-integrated, small-cell-style gNB.
///
/// \cond

static std::string config_file;

static std::atomic<bool> is_running = {true};
// NGAP configuration.
static srsran::sctp_network_gateway_config ngap_nw_config;
const std::string                          srsgnb_version = "0.1";

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(buffer, "srsRAN 5G gNB version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false);
}

/// This function takes the populated appconfig and generates (sub)-component configurations.
static void compute_derived_args(const gnb_appconfig& gnb_params)
{
  /// Simply set the respective values in the appconfig.
  ngap_nw_config.connect_address = gnb_params.amf_cfg.ip_addr;
  ngap_nw_config.connect_port    = gnb_params.amf_cfg.port;
  ngap_nw_config.bind_address    = gnb_params.amf_cfg.bind_addr;
  ngap_nw_config.ppid            = NGAP_PPID;
  if (gnb_params.amf_cfg.sctp_rto_initial >= 0) {
    ngap_nw_config.rto_initial = gnb_params.amf_cfg.sctp_rto_initial;
  }
  if (gnb_params.amf_cfg.sctp_rto_min >= 0) {
    ngap_nw_config.rto_min = gnb_params.amf_cfg.sctp_rto_min;
  }
  if (gnb_params.amf_cfg.sctp_rto_max >= 0) {
    ngap_nw_config.rto_max = gnb_params.amf_cfg.sctp_rto_max;
  }
  if (gnb_params.amf_cfg.sctp_init_max_attempts >= 0) {
    ngap_nw_config.init_max_attempts = gnb_params.amf_cfg.sctp_init_max_attempts;
  }
  if (gnb_params.amf_cfg.sctp_max_init_timeo >= 0) {
    ngap_nw_config.max_init_timeo = gnb_params.amf_cfg.sctp_max_init_timeo;
  }
}

namespace {

/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell_) : cell(cell_) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

class fapi_slot_last_message_dummy : public fapi::slot_last_message_notifier
{
public:
  void on_last_message(slot_point slot) override {}
};

/// Manages the workers of the app.
struct worker_manager {
  worker_manager(const gnb_appconfig& appcfg)
  {
    optional<lower_phy_thread_profile> lower_phy_profile;
    std::string                        driver = "";
    if (variant_holds_alternative<ru_sdr_appconfig>(appcfg.ru_cfg)) {
      const ru_sdr_appconfig& sdr_cfg = variant_get<ru_sdr_appconfig>(appcfg.ru_cfg);
      driver                          = sdr_cfg.device_driver;

      lower_phy_profile =
          (driver != "zmq") ? sdr_cfg.expert_cfg.lphy_executor_profile : lower_phy_thread_profile::blocking;
    }

    if (appcfg.expert_phy_cfg.nof_pdsch_threads > 1) {
      create_worker_pool("pdsch",
                         appcfg.expert_phy_cfg.nof_pdsch_threads,
                         2 * MAX_CBS_PER_PDU,
                         os_thread_realtime_priority::max() - 10);
    }

    create_executors(driver == "zmq", lower_phy_profile, appcfg.expert_phy_cfg.nof_ul_threads);
  }

  void stop()
  {
    du_cell_worker->stop();
    gnb_ctrl_worker->stop();
    for (auto& worker : workers) {
      worker.second->stop();
    }
    for (auto& pool : worker_pools) {
      pool.second->stop();
    }
  }

  /*
  du ctrl exec points to general ctrl_worker
  du ue exec points to the general ue_worker
  cu-cp ctrl exec points to general ctrl_worker (just like the du ctrl exec)
  cu-up ue exec points to the general ue_worker (just like the du ue exec)

  The handler side is responsible for executor dispatching:
  - ngap::handle_message calls cu-cp ctrl exec
  - f1ap_cu::handle_message calls cu-cp ctrl exec
  - e1ap_cu_cp::handle_message calls cu-cp ctrl exec
  - e1ap_cu_up::handle_message calls cu-up ue exec
  */

  std::unique_ptr<task_executor> cu_cp_exec;
  std::unique_ptr<task_executor> cu_up_exec;
  std::unique_ptr<task_executor> gtpu_pdu_exec;
  std::unique_ptr<task_executor> du_ctrl_exec;
  std::unique_ptr<task_executor> du_timer_exec;
  std::unique_ptr<task_executor> du_ue_exec;
  std::unique_ptr<task_executor> du_cell_exec;
  std::unique_ptr<task_executor> du_slot_exec;
  std::unique_ptr<task_executor> lower_phy_tx_exec;
  std::unique_ptr<task_executor> lower_phy_rx_exec;
  std::unique_ptr<task_executor> lower_phy_dl_exec;
  std::unique_ptr<task_executor> lower_phy_ul_exec;
  std::unique_ptr<task_executor> lower_prach_exec;
  std::unique_ptr<task_executor> upper_dl_exec;
  std::unique_ptr<task_executor> upper_pusch_exec;
  std::unique_ptr<task_executor> upper_pucch_exec;
  std::unique_ptr<task_executor> upper_prach_exec;
  std::unique_ptr<task_executor> upper_pdsch_exec;
  std::unique_ptr<task_executor> radio_exec;
  std::unique_ptr<task_executor> ru_printer_exec;
  std::unique_ptr<task_executor> ru_timing_exec;
  std::unique_ptr<task_executor> ru_tx_exec;
  std::unique_ptr<task_executor> ru_rx_exec;

  std::unordered_map<std::string, std::unique_ptr<task_executor>> task_execs;
  std::unique_ptr<du_high_executor_mapper>                        du_high_exec_mapper;

private:
  using du_cell_worker_type  = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;
  using gnb_ctrl_worker_type = priority_multiqueue_task_worker<task_queue_policy::spsc, task_queue_policy::blocking>;

  std::unique_ptr<du_cell_worker_type>                               du_cell_worker;
  std::unique_ptr<gnb_ctrl_worker_type>                              gnb_ctrl_worker;
  std::unordered_map<std::string, std::unique_ptr<task_worker>>      workers;
  std::unordered_map<std::string, std::unique_ptr<task_worker_pool>> worker_pools;

  // helper method to create workers
  template <typename... Args>
  void create_worker(const std::string& name, Args&&... args)
  {
    auto ret = workers.insert(std::make_pair(name, std::make_unique<task_worker>(name, std::forward<Args>(args)...)));
    srsran_assert(ret.second, "Unable to create worker {}.", name);
  }
  // helper method to create worker pool
  void create_worker_pool(const std::string&          name,
                          size_t                      nof_workers,
                          size_t                      queue_size,
                          os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime())
  {
    auto ret = worker_pools.insert(
        std::make_pair(name, std::make_unique<task_worker_pool>(nof_workers, queue_size, name, prio)));
    srsran_assert(ret.second, "Unable to create worker pool {}.", name);
  }

  void create_executors(bool                               blocking_mode_active,
                        optional<lower_phy_thread_profile> lower_phy_profile,
                        unsigned                           nof_ul_workers)
  {
    static const uint32_t task_worker_queue_size = 2048;

    // Instantiate workers
    create_worker("gnb_ue", 512);
    gnb_ctrl_worker = std::make_unique<du_cell_worker_type>("gnb_ctrl",
                                                            std::array<unsigned, 2>{64, task_worker_queue_size},
                                                            std::chrono::microseconds{100},
                                                            os_thread_realtime_priority::max() - 2,
                                                            os_sched_affinity_bitmask{});
    du_cell_worker  = std::make_unique<du_cell_worker_type>("du_cell",
                                                           std::array<unsigned, 2>{8, task_worker_queue_size},
                                                           std::chrono::microseconds{10},
                                                           os_thread_realtime_priority::max() - 2,
                                                           os_sched_affinity_bitmask{});

    if (blocking_mode_active) {
      create_worker("phy_worker", task_worker_queue_size, os_thread_realtime_priority::max());
    } else {
      create_worker("phy_prach", task_worker_queue_size, os_thread_realtime_priority::max() - 2);
      create_worker("upper_phy_dl", task_worker_queue_size, os_thread_realtime_priority::max() - 10);
      create_worker_pool(
          "upper_phy_ul", nof_ul_workers, task_worker_queue_size, os_thread_realtime_priority::max() - 20);
    }

    if (!blocking_mode_active && lower_phy_profile && *lower_phy_profile == lower_phy_thread_profile::blocking) {
      create_worker("phy_worker", task_worker_queue_size, os_thread_realtime_priority::max());
    }

    create_worker("radio", task_worker_queue_size);
    create_worker("ru_stats_worker", 1);

    // Instantiate task executors
    cu_cp_exec      = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
    cu_up_exec      = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));
    gtpu_pdu_exec   = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"), false);
    du_ctrl_exec    = make_priority_task_executor_ptr<task_queue_priority::min>(*gnb_ctrl_worker);
    du_timer_exec   = make_priority_task_executor_ptr<task_queue_priority::max>(*gnb_ctrl_worker);
    du_ue_exec      = std::make_unique<task_worker_executor>(*workers.at("gnb_ue"));
    du_cell_exec    = make_priority_task_executor_ptr<task_queue_priority::min>(*du_cell_worker);
    ru_printer_exec = std::make_unique<task_worker_executor>(*workers.at("ru_stats_worker"));
    if (blocking_mode_active) {
      du_slot_exec = make_sync_executor(make_priority_task_worker_executor<task_queue_priority::max>(*du_cell_worker));
      task_worker& phy_worker = *workers.at("phy_worker");
      lower_phy_tx_exec       = std::make_unique<task_worker_executor>(phy_worker);
      lower_phy_rx_exec       = std::make_unique<task_worker_executor>(phy_worker);
      lower_phy_dl_exec       = std::make_unique<task_worker_executor>(phy_worker);
      lower_phy_ul_exec       = std::make_unique<task_worker_executor>(phy_worker);
      lower_prach_exec        = std::make_unique<task_worker_executor>(phy_worker);
      upper_dl_exec           = std::make_unique<task_worker_executor>(phy_worker);
      upper_pusch_exec        = std::make_unique<task_worker_executor>(phy_worker);
      upper_pucch_exec        = std::make_unique<task_worker_executor>(phy_worker);
      upper_prach_exec        = std::make_unique<task_worker_executor>(phy_worker);
    } else {
      du_slot_exec     = make_priority_task_executor_ptr<task_queue_priority::max>(*du_cell_worker);
      lower_prach_exec = std::make_unique<task_worker_executor>(*workers.at("phy_prach"));
      upper_dl_exec    = std::make_unique<task_worker_executor>(*workers.at("upper_phy_dl"));
      upper_pusch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("upper_phy_ul"));
      upper_pucch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("upper_phy_ul"));
      upper_prach_exec = std::make_unique<task_worker_executor>(*workers.at("phy_prach"));
    }

    if (worker_pools.count("pdsch")) {
      upper_pdsch_exec = std::make_unique<task_worker_pool_executor>(*worker_pools.at("pdsch"));
    }

    if (lower_phy_profile) {
      switch (*lower_phy_profile) {
        case lower_phy_thread_profile::blocking: {
          fmt::print("Lower PHY in executor blocking mode.\n");
          task_worker& phy_worker = *workers.at("phy_worker");
          lower_phy_tx_exec       = std::make_unique<task_worker_executor>(phy_worker);
          lower_phy_rx_exec       = std::make_unique<task_worker_executor>(phy_worker);
          lower_phy_dl_exec       = std::make_unique<task_worker_executor>(phy_worker);
          lower_phy_ul_exec       = std::make_unique<task_worker_executor>(phy_worker);
          break;
        }
        case lower_phy_thread_profile::single: {
          fmt::print("Lower PHY in single executor mode.\n");
          create_worker("lower_phy", 128, os_thread_realtime_priority::max());
          task_worker& lower_phy_worker = *workers.at("lower_phy");
          lower_phy_tx_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
          lower_phy_rx_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
          lower_phy_dl_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
          lower_phy_ul_exec             = std::make_unique<task_worker_executor>(lower_phy_worker);
          break;
        }
        case lower_phy_thread_profile::dual: {
          fmt::print("Lower PHY in dual executor mode.\n");
          create_worker("lower_phy_dl", 128, os_thread_realtime_priority::max());
          create_worker("lower_phy_ul", 2, os_thread_realtime_priority::max() - 1);
          lower_phy_tx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
          lower_phy_rx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
          lower_phy_dl_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
          lower_phy_ul_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
          break;
        }
        case lower_phy_thread_profile::quad: {
          fmt::print("Lower PHY in quad executor mode.\n");
          create_worker("lower_phy_tx", 128, os_thread_realtime_priority::max());
          create_worker("lower_phy_rx", 1, os_thread_realtime_priority::max() - 2);
          create_worker("lower_phy_dl", 128, os_thread_realtime_priority::max() - 1);
          create_worker("lower_phy_ul", 128, os_thread_realtime_priority::max() - 3);
          lower_phy_tx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_tx"));
          lower_phy_rx_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_rx"));
          lower_phy_dl_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_dl"));
          lower_phy_ul_exec = std::make_unique<task_worker_executor>(*workers.at("lower_phy_ul"));
          break;
        }
      }
    }

    radio_exec = std::make_unique<task_worker_executor>(*workers.at("radio"));

    // RU executors.
    create_worker("ru_timing", 1, os_thread_realtime_priority::max() - 1, os_sched_affinity_bitmask(0));
    ru_timing_exec = std::make_unique<task_worker_executor>(*workers.at("ru_timing"));
    create_worker("ru_tx", 128, os_thread_realtime_priority::max() - 3, os_sched_affinity_bitmask(1));
    ru_tx_exec = std::make_unique<task_worker_executor>(*workers.at("ru_tx"));
    create_worker("ru_rx", 1, os_thread_realtime_priority::max() - 2, os_sched_affinity_bitmask(3));
    ru_rx_exec = std::make_unique<task_worker_executor>(*workers.at("ru_rx"));

    // Executor mappers.
    du_high_exec_mapper = std::make_unique<du_high_executor_mapper_impl>(
        std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{du_cell_exec.get()},
                                               std::initializer_list<task_executor*>{du_slot_exec.get()}),
        std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{du_ue_exec.get()}),
        *du_ctrl_exec,
        *du_timer_exec);
  }
};

} // namespace

static void local_signal_handler()
{
  is_running = false;
}

static fapi::prach_config generate_prach_config_tlv(const std::vector<du_cell_config>& cell_cfg)
{
  srsran_assert(cell_cfg.size() == 1, "Currently supporting one cell");

  const du_cell_config& cell = cell_cfg.front();

  fapi::prach_config config     = {};
  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::kHz1_25;
  config.prach_ul_bwp_pusch_scs = cell.scs_common;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm;
  config.prach_config_index =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index;
  config.prach_format           = prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.emplace_back();
  fd_occasion.prach_root_sequence_index = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index;
  fd_occasion.prach_freq_offset =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start;
  fd_occasion.prach_zero_corr_conf =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv(const gnb_appconfig& config)
{
  // Deduce common numerology and grid size for DL and UL.
  unsigned numerology       = to_numerology_value(config.common_cell_cfg.common_scs);
  unsigned grid_size_bw_prb = band_helper::get_n_rbs_from_bw(
      config.common_cell_cfg.channel_bw_mhz,
      config.common_cell_cfg.common_scs,
      band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(config.common_cell_cfg.dl_arfcn)));

  fapi::carrier_config fapi_config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid and nof_prb_dl_grid for the common SCS.
  fapi_config.dl_grid_size             = {};
  fapi_config.dl_grid_size[numerology] = grid_size_bw_prb;
  fapi_config.ul_grid_size             = {};
  fapi_config.ul_grid_size[numerology] = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  fapi_config.num_tx_ant = config.common_cell_cfg.nof_antennas_dl;
  fapi_config.num_rx_ant = config.common_cell_cfg.nof_antennas_ul;

  return fapi_config;
}

/// Resolves the generic Radio Unit dependencies and adds them to the configuration.
static void configure_ru_generic_executors_and_notifiers(ru_generic_configuration&           config,
                                                         const log_appconfig&                log_cfg,
                                                         worker_manager&                     workers,
                                                         ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                         ru_timing_notifier&                 timing_notifier)
{
  srslog::basic_logger& rf_logger = srslog::fetch_basic_logger("RF", false);
  rf_logger.set_level(srslog::str_to_basic_level(log_cfg.radio_level));

  config.rf_logger                             = &rf_logger;
  config.radio_exec                            = workers.radio_exec.get();
  config.lower_phy_config.tx_task_executor     = workers.lower_phy_tx_exec.get();
  config.lower_phy_config.rx_task_executor     = workers.lower_phy_rx_exec.get();
  config.lower_phy_config.dl_task_executor     = workers.lower_phy_dl_exec.get();
  config.lower_phy_config.ul_task_executor     = workers.lower_phy_ul_exec.get();
  config.lower_phy_config.prach_async_executor = workers.lower_prach_exec.get();
  config.statistics_printer_executor           = workers.ru_printer_exec.get();
  config.timing_notifier                       = &timing_notifier;
  config.symbol_notifier                       = &symbol_notifier;
}

/// Resolves the Open Fronthaul Radio Unit dependencies and adds them to the configuration.
static void configure_ru_ofh_executors_and_notifiers(ru_ofh_configuration&               config,
                                                     const log_appconfig&                log_cfg,
                                                     worker_manager&                     workers,
                                                     ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                     ru_timing_notifier&                 timing_notifier)
{
  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));

  config.logger             = &ofh_logger;
  config.rt_timing_executor = workers.ru_timing_exec.get();
  config.timing_notifier    = &timing_notifier;
  config.rx_symbol_notifier = &symbol_notifier;

  // Configure sector.
  ru_ofh_sector_configuration& sector_cfg = config.sector_configs.front();
  sector_cfg.receiver_executor            = workers.ru_rx_exec.get();
  sector_cfg.transmitter_executor         = workers.ru_tx_exec.get();
}

/// Resolves the Radio Unit dependencies and adds them to the configuration.
static void configure_ru_executors_and_notifiers(ru_configuration&                   config,
                                                 const log_appconfig&                log_cfg,
                                                 worker_manager&                     workers,
                                                 ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                 ru_timing_notifier&                 timing_notifier)
{
  if (variant_holds_alternative<ru_ofh_configuration>(config.config)) {
    configure_ru_ofh_executors_and_notifiers(
        variant_get<ru_ofh_configuration>(config.config), log_cfg, workers, symbol_notifier, timing_notifier);

    return;
  }

  configure_ru_generic_executors_and_notifiers(
      variant_get<ru_generic_configuration>(config.config), log_cfg, workers, symbol_notifier, timing_notifier);
}

int main(int argc, char** argv)
{
  // Set signal handler.
  register_signal_handler(local_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup size of byte buffer pool.
  init_byte_buffer_segment_pool(524288);

  // Setup and configure config parsing.
  CLI::App app("srsGNB application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  gnb_appconfig gnb_cfg;
  // Configure CLI11 with the gNB application configuration schema.
  configure_cli11_with_gnb_appconfig_schema(app, gnb_cfg);

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg)) {
    report_error("Invalid configuration detected.\n");
  }

  // Compute derived parameters.
  compute_derived_args(gnb_cfg);

  // Set up logging.
  srslog::sink* log_sink = (gnb_cfg.log_cfg.filename == "stdout") ? srslog::create_stdout_sink()
                                                                  : srslog::create_file_sink(gnb_cfg.log_cfg.filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  srslog::set_default_sink(*log_sink);
  srslog::init();

  // Set log-level of app and all non-layer specific components to app level.
  srslog::basic_logger& gnb_logger = srslog::fetch_basic_logger("GNB");
  for (const auto& id : {"GNB", "ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.lib_level));
    logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  // Set component-specific logging options.
  for (const auto& id : {"DU", "DU-MNG", "UE-MNG"}) {
    auto& du_logger = srslog::fetch_basic_logger(id, false);
    du_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.du_level));
    du_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  for (const auto& id : {"CU-CP", "CU-UEMNG", "CU-CP-E1"}) {
    auto& cu_cp_logger = srslog::fetch_basic_logger(id, false);
    cu_cp_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.cu_level));
    cu_cp_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  for (const auto& id : {"CU-UP", "CU-UP-E1"}) {
    auto& cu_up_logger = srslog::fetch_basic_logger(id, false);
    cu_up_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.cu_level));
    cu_up_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  // Set layer-specific logging options.
  auto& phy_logger = srslog::fetch_basic_logger("PHY", false);
  phy_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.phy_level));
  phy_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  for (const auto& id : {"MAC", "SCHED"}) {
    auto& mac_logger = srslog::fetch_basic_logger(id, true);
    mac_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.mac_level));
    mac_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  auto& rlc_logger = srslog::fetch_basic_logger("RLC", false);
  rlc_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.rlc_level));
  rlc_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& du_f1ap_logger = srslog::fetch_basic_logger("DU-F1", false);
  auto& cu_f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1", false);
  du_f1ap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1ap_level));
  du_f1ap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  cu_f1ap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1ap_level));
  cu_f1ap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  for (const auto& id : {"CU-F1-U", "DU-F1-U"}) {
    auto& f1u_logger = srslog::fetch_basic_logger(id, false);
    f1u_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1u_level));
    f1u_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  auto& sec_logger = srslog::fetch_basic_logger("SEC", false);
  sec_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.sec_level));
  sec_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& pdcp_logger = srslog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.rrc_level));
  rrc_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& ngap_logger = srslog::fetch_basic_logger("NGAP", false);
  ngap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.ngap_level));
  ngap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& sdap_logger = srslog::fetch_basic_logger("SDAP", false);
  sdap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.sdap_level));
  sdap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& gtpu_logger = srslog::fetch_basic_logger("GTPU", false);
  gtpu_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.gtpu_level));
  gtpu_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& fapi_logger = srslog::fetch_basic_logger("FAPI", true);
  fapi_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.fapi_level));

  // Log build info
  gnb_logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    gnb_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    gnb_logger.error("The CPU does not support the required CPU features that were configured during compile time: {}",
                     get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and
  // print a warning if required.
  check_cpu_governor(gnb_logger);
  check_drm_kms_polling(gnb_logger);

  // Set layer-specific pcap options.
  std::unique_ptr<dlt_pcap> ngap_p = std::make_unique<dlt_pcap_impl>(PCAP_NGAP_DLT, "NGAP");
  if (gnb_cfg.pcap_cfg.ngap.enabled) {
    ngap_p->open(gnb_cfg.pcap_cfg.ngap.filename.c_str());
  }
  std::unique_ptr<dlt_pcap> e1ap_p = std::make_unique<dlt_pcap_impl>(PCAP_E1AP_DLT, "E1AP");
  if (gnb_cfg.pcap_cfg.e1ap.enabled) {
    e1ap_p->open(gnb_cfg.pcap_cfg.e1ap.filename.c_str());
  }
  std::unique_ptr<dlt_pcap> f1ap_p = std::make_unique<dlt_pcap_impl>(PCAP_F1AP_DLT, "F1AP");
  if (gnb_cfg.pcap_cfg.f1ap.enabled) {
    f1ap_p->open(gnb_cfg.pcap_cfg.f1ap.filename.c_str());
  }
  std::unique_ptr<mac_pcap> mac_p = std::make_unique<mac_pcap_impl>();
  if (gnb_cfg.pcap_cfg.mac.enabled) {
    mac_p->open(gnb_cfg.pcap_cfg.mac.filename.c_str());
  }

  worker_manager workers{gnb_cfg};

  f1ap_local_adapter f1ap_cu_to_du_adapter("CU-CP-F1", *f1ap_p), f1ap_du_to_cu_adapter("DU-F1", *f1ap_p);
  e1ap_local_adapter e1ap_cp_to_up_adapter("CU-CP", *e1ap_p), e1ap_up_to_cp_adapter("CU-UP", *e1ap_p);

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager app_timers{256};

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Create IO broker.
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll);

  // Create console helper object for commands and metrics printing.
  gnb_console_helper console(*epoll_broker);
  console.on_app_starting();

  // Create NGAP adapter.
  std::unique_ptr<srsran::srs_cu_cp::ngap_network_adapter> ngap_adapter =
      std::make_unique<srsran::srs_cu_cp::ngap_network_adapter>(*epoll_broker, *ngap_p);

  // Create SCTP network adapter.
  std::unique_ptr<sctp_network_gateway> sctp_gateway = {};
  if (not gnb_cfg.amf_cfg.no_core) {
    gnb_logger.info("Connecting to AMF ({})..", ngap_nw_config.connect_address, ngap_nw_config.connect_port);
    sctp_gateway = create_sctp_network_gateway({ngap_nw_config, *ngap_adapter, *ngap_adapter});

    // Connect NGAP adapter to SCTP network gateway.
    ngap_adapter->connect_gateway(sctp_gateway.get(), sctp_gateway.get());
    gnb_logger.info("AMF connection established");
  } else {
    gnb_logger.info("Bypassing AMF connection");
  }
  // Create CU-UP config.
  srsran::srs_cu_up::cu_up_configuration cu_up_cfg;
  cu_up_cfg.cu_up_executor       = workers.cu_up_exec.get();
  cu_up_cfg.gtpu_pdu_executor    = workers.gtpu_pdu_exec.get();
  cu_up_cfg.e1ap_notifier        = &e1ap_up_to_cp_adapter;
  cu_up_cfg.f1u_gateway          = f1u_conn->get_f1u_cu_up_gateway();
  cu_up_cfg.epoll_broker         = epoll_broker.get();
  cu_up_cfg.timers               = &app_timers;
  cu_up_cfg.net_cfg.n3_bind_addr = gnb_cfg.amf_cfg.bind_addr; // TODO: rename variable to core addr
  cu_up_cfg.net_cfg.f1u_bind_addr =
      gnb_cfg.amf_cfg.bind_addr; // FIXME: check if this can be removed for co-located case

  // create and start DUT
  std::unique_ptr<srsran::srs_cu_up::cu_up_interface> cu_up_obj = create_cu_up(cu_up_cfg);

  // Create CU-CP config.
  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(gnb_cfg);
  cu_cp_cfg.cu_cp_executor                 = workers.cu_cp_exec.get();
  cu_cp_cfg.f1ap_notifier                  = &f1ap_cu_to_du_adapter;
  cu_cp_cfg.e1ap_notifier                  = &e1ap_cp_to_up_adapter;
  cu_cp_cfg.ngap_notifier                  = ngap_adapter.get();

  // create CU-CP.
  std::unique_ptr<srsran::srs_cu_cp::cu_cp_interface> cu_cp_obj = create_cu_cp(cu_cp_cfg);
  cu_cp_obj->handle_new_du_connection();    // trigger DU addition
  cu_cp_obj->handle_new_cu_up_connection(); // trigger CU-UP addition

  // Connect NGAP adpter to CU-CP to pass NGAP messages.
  ngap_adapter->connect_ngap(&cu_cp_obj->get_ngap_message_handler(), &cu_cp_obj->get_ngap_event_handler());

  // attach E1AP adapters to CU-UP and CU-CP
  e1ap_up_to_cp_adapter.attach_handler(&cu_cp_obj->get_e1ap_message_handler(srsran::srs_cu_cp::uint_to_cu_up_index(0)));
  e1ap_cp_to_up_adapter.attach_handler(&cu_up_obj->get_e1ap_message_handler());

  // start CU-CP
  gnb_logger.info("Starting CU-CP...");
  cu_cp_obj->start();
  gnb_logger.info("CU-CP started successfully");

  // Radio Unit instantiation block.
  ru_configuration ru_cfg = generate_ru_config(gnb_cfg);

  upper_ru_ul_adapter     ru_ul_adapt;
  upper_ru_timing_adapter ru_timing_adapt;

  configure_ru_executors_and_notifiers(ru_cfg, gnb_cfg.log_cfg, workers, ru_ul_adapt, ru_timing_adapt);

  std::unique_ptr<radio_unit> ru_object;
  if (variant_holds_alternative<ru_ofh_configuration>(ru_cfg.config)) {
    ru_object = create_ofh_ru(variant_get<ru_ofh_configuration>(ru_cfg.config));
  } else {
    ru_object = create_generic_ru(variant_get<ru_generic_configuration>(ru_cfg.config));
  }
  report_error_if_not(ru_object, "Unable to create Radio Unit.");
  gnb_logger.info("Radio Unit created successfully");

  upper_ru_dl_rg_adapter      ru_dl_rg_adapt;
  upper_ru_ul_request_adapter ru_ul_request_adapt;
  ru_dl_rg_adapt.connect(ru_object->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru_object->get_uplink_plane_handler());

  // Upper PHY instantiation block.
  auto upper = create_upper_phy(gnb_cfg,
                                &ru_dl_rg_adapt,
                                workers.upper_dl_exec.get(),
                                workers.upper_pucch_exec.get(),
                                workers.upper_pusch_exec.get(),
                                workers.upper_prach_exec.get(),
                                workers.upper_pdsch_exec.get(),
                                &ru_ul_request_adapt);
  report_error_if_not(upper, "Unable to create upper PHY.");
  gnb_logger.info("Upper PHY created successfully");

  // Make connections between upper and RU.
  ru_ul_adapt.connect(upper->get_rx_symbol_handler());
  ru_timing_adapt.connect(upper->get_timing_handler());

  // Create FAPI adaptors.
  std::vector<du_cell_config> du_cfg = generate_du_cell_config(gnb_cfg);
  unsigned                    sector = du_cfg.size() - 1;
  subcarrier_spacing          scs    = du_cfg.front().scs_common;

  auto phy_adaptor = build_phy_fapi_adaptor(sector,
                                            scs,
                                            scs,
                                            upper->get_downlink_processor_pool(),
                                            upper->get_downlink_resource_grid_pool(),
                                            upper->get_uplink_request_processor(),
                                            upper->get_uplink_resource_grid_pool(),
                                            upper->get_uplink_slot_pdu_repository(),
                                            upper->get_downlink_pdu_validator(),
                                            upper->get_uplink_pdu_validator(),
                                            generate_prach_config_tlv(du_cfg),
                                            generate_carrier_config_tlv(gnb_cfg));
  report_error_if_not(phy_adaptor, "Unable to create PHY adaptor.");
  upper->set_rx_results_notifier(phy_adaptor->get_rx_results_notifier());
  upper->set_timing_notifier(phy_adaptor->get_timing_notifier());

  fapi_slot_last_message_dummy                      last_msg_dummy;
  std::unique_ptr<fapi::slot_message_gateway>       logging_slot_gateway;
  std::unique_ptr<fapi::slot_data_message_notifier> logging_slot_data_notifier;
  std::unique_ptr<fapi::slot_time_message_notifier> logging_slot_time_notifier;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>   mac_adaptor;
  if (gnb_cfg.log_cfg.fapi_level == "debug") {
    // Create gateway loggers and intercept MAC adaptor calls.
    logging_slot_gateway = fapi::create_logging_slot_gateway(phy_adaptor->get_slot_message_gateway());
    report_error_if_not(logging_slot_gateway, "Unable to create logger for slot data notifications.");
    mac_adaptor = build_mac_fapi_adaptor(0, scs, *logging_slot_gateway, last_msg_dummy);

    // Create notification loggers.
    logging_slot_data_notifier = fapi::create_logging_slot_data_notifier(mac_adaptor->get_slot_data_notifier());
    report_error_if_not(logging_slot_data_notifier, "Unable to create logger for slot data notifications.");
    logging_slot_time_notifier = fapi::create_logging_slot_time_notifier(mac_adaptor->get_slot_time_notifier());
    report_error_if_not(logging_slot_time_notifier, "Unable to create logger for slot time notifications.");

    // Connect the PHY adaptor with the loggers to intercept PHY notifications.
    phy_adaptor->set_slot_time_message_notifier(*logging_slot_time_notifier);
    phy_adaptor->set_slot_data_message_notifier(*logging_slot_data_notifier);
  } else {
    mac_adaptor = build_mac_fapi_adaptor(0, scs, phy_adaptor->get_slot_message_gateway(), last_msg_dummy);
    report_error_if_not(mac_adaptor, "Unable to create MAC adaptor.");
    phy_adaptor->set_slot_time_message_notifier(mac_adaptor->get_slot_time_notifier());
    phy_adaptor->set_slot_data_message_notifier(mac_adaptor->get_slot_data_notifier());
  }
  gnb_logger.info("FAPI adaptors created successfully");

  // DU QoS config
  const std::map<five_qi_t, du_qos_config>& du_qos_cfg = generate_du_qos_config(gnb_cfg);
  for (const auto& it : du_qos_cfg) {
    gnb_logger.debug("QoS RLC configuration: 5QI={} RLC={}", it.first, it.second.rlc);
  }

  // Cell configuration.
  phy_dummy phy(mac_adaptor->get_cell_result_notifier());

  srs_du::du_high_configuration du_hi_cfg = {};
  du_hi_cfg.exec_mapper                   = workers.du_high_exec_mapper.get();
  du_hi_cfg.f1ap_notifier                 = &f1ap_du_to_cu_adapter;
  du_hi_cfg.f1u_gw                        = f1u_conn->get_f1u_du_gateway();
  du_hi_cfg.phy_adapter                   = &phy;
  du_hi_cfg.timers                        = &app_timers;
  du_hi_cfg.cells                         = du_cfg;
  du_hi_cfg.qos                           = du_qos_cfg;
  du_hi_cfg.pcap                          = mac_p.get();
  du_hi_cfg.mac_cfg                       = generate_mac_expert_config(gnb_cfg);
  du_hi_cfg.metrics_notifier              = &console.get_metrics_notifier();
  du_hi_cfg.sched_cfg                     = generate_scheduler_expert_config(gnb_cfg);
  if (gnb_cfg.test_mode_cfg.test_ue.rnti != INVALID_RNTI) {
    du_hi_cfg.test_cfg.test_ue = srs_du::du_test_config::test_ue_config{gnb_cfg.test_mode_cfg.test_ue.rnti,
                                                                        gnb_cfg.test_mode_cfg.test_ue.pdsch_active,
                                                                        gnb_cfg.test_mode_cfg.test_ue.pusch_active};
  }

  srs_du::du_high du_obj(du_hi_cfg);
  gnb_logger.info("DU-High created successfully");

  // attach F1AP adapter to DU and CU-CP
  f1ap_du_to_cu_adapter.attach_handler(&cu_cp_obj->get_f1ap_message_handler(srsran::srs_cu_cp::uint_to_du_index(0)));
  f1ap_cu_to_du_adapter.attach_handler(&du_obj.get_f1ap_message_handler());

  // Start execution.
  gnb_logger.info("Starting DU-High...");
  du_obj.start();
  gnb_logger.info("DU-High started successfully");

  // Give some time to the MAC to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Configure the DU slot handler.
  du_cell_index_t cell_id = to_du_cell_index(0);
  mac_adaptor->set_cell_slot_handler(du_obj.get_slot_handler(cell_id));
  mac_adaptor->set_cell_rach_handler(du_obj.get_rach_handler(cell_id));
  mac_adaptor->set_cell_pdu_handler(du_obj.get_pdu_handler(cell_id));
  mac_adaptor->set_cell_crc_handler(du_obj.get_control_information_handler(cell_id));

  // Start processing.
  gnb_logger.info("Starting Radio Unit...");
  ru_object->get_controller().start();
  gnb_logger.info("Radio Unit started successfully");

  console.set_cells(du_hi_cfg.cells);
  console.on_app_running();

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  console.on_app_stopping();

  ngap_p->close();
  e1ap_p->close();
  f1ap_p->close();
  mac_p->close();

  gnb_logger.info("Stopping Radio Unit...");
  ru_object->get_controller().stop();
  gnb_logger.info("Radio Unit notify_stop successfully");

  gnb_logger.info("Closing DU-high...");
  du_obj.stop();
  gnb_logger.info("DU-high closed successfully");

  if (not gnb_cfg.amf_cfg.no_core) {
    gnb_logger.info("Closing network connections...");
    ngap_adapter->disconnect_gateway();
    gnb_logger.info("Network connections closed successfully");
  }

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors notify_stop successfully");

  srslog::flush();

  return 0;
}

/// \endcond
